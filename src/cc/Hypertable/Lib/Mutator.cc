/**
 * Copyright (C) 2007 Doug Judd (Zvents, Inc.)
 * 
 * This file is part of Hypertable.
 * 
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 * 
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cstring>

extern "C" {
#include <poll.h>
}

#include "Common/StringExt.h"

#include "Key.h"
#include "Mutator.h"

namespace {
  const uint64_t DEFAULT_MAX_MEMORY = 20000000LL;
}


/**
 * 
 */
Mutator::Mutator(PropertiesPtr &props_ptr, Comm *comm, TableIdentifierT *table_identifier, SchemaPtr &schema_ptr, RangeLocatorPtr &range_locator_ptr) : m_props_ptr(props_ptr), m_comm(comm), m_schema_ptr(schema_ptr), m_range_locator_ptr(range_locator_ptr), m_table_name(table_identifier->name), m_memory_used(0), m_max_memory(DEFAULT_MAX_MEMORY) {
  // copy TableIdentifierT
  memcpy(&m_table_identifier, table_identifier, sizeof(TableIdentifierT));
  m_table_identifier.name = m_table_name.c_str();

  m_buffer_ptr = new MutatorScatterBuffer(props_ptr, m_comm, &m_table_identifier, m_schema_ptr, m_range_locator_ptr);

}


/**
 * 
 */
void Mutator::set(uint64_t timestamp, KeySpec &key, uint8_t *value, uint32_t value_len) {
  int error;

  sanity_check_key(key);

  if (key.column_family == 0)
    throw Exception(Error::BAD_KEY, "Invalid key - column family not specified");

  {
    Key full_key;
    Schema::ColumnFamily *cf = m_schema_ptr->get_column_family(key.column_family);
    if (cf == 0)
      throw Exception(Error::BAD_KEY, (std::string)"Invalid key - bad column family '" + key.column_family + "'");
    full_key.row = (const char *)key.row;
    full_key.column_qualifier = (const char *)key.column_qualifier;
    full_key.column_family_code = (uint8_t)cf->id;
    full_key.timestamp = timestamp;
    
    if ((error = m_buffer_ptr->set(full_key, value, value_len)) != Error::OK) {
      LOG_ERROR("Problem setting cell/value");
    }
  }

  m_memory_used += 20 + key.row_len + key.column_qualifier_len + value_len;

  if (m_buffer_ptr->full() || m_memory_used > m_max_memory) {

    if (m_prev_buffer_ptr)
      wait_for_previous_buffer();

    m_buffer_ptr->send();

    m_prev_buffer_ptr = m_buffer_ptr;

    m_buffer_ptr = new MutatorScatterBuffer(m_props_ptr, m_comm, &m_table_identifier, m_schema_ptr, m_range_locator_ptr);
    m_memory_used = 0;
  }

}



void Mutator::set_delete(uint64_t timestamp, KeySpec &key) {
  int error;
  Key full_key;

  sanity_check_key(key);

  if (key.column_family == 0) {
    full_key.row = (const char *)key.row;
    full_key.column_family_code = 0;
    full_key.column_qualifier = 0;
    full_key.timestamp = timestamp;    
  }
  else  {
    Schema::ColumnFamily *cf = m_schema_ptr->get_column_family(key.column_family);
    if (cf == 0)
      throw Exception(Error::BAD_KEY, (std::string)"Invalid key - bad column family '" + key.column_family + "'");
    full_key.row = (const char *)key.row;
    full_key.column_qualifier = (const char *)key.column_qualifier;
    full_key.column_family_code = (uint8_t)cf->id;
    full_key.timestamp = timestamp;
  }

  if ((error = m_buffer_ptr->set_delete(full_key)) != Error::OK) {
    LOG_VA_ERROR("Problem doing delete - %s", Error::get_text(error));
  }

  m_memory_used += 20 + key.row_len + key.column_qualifier_len;

  if (m_buffer_ptr->full() || m_memory_used > m_max_memory) {

    if (m_prev_buffer_ptr)
      wait_for_previous_buffer();

    m_buffer_ptr->send();

    m_prev_buffer_ptr = m_buffer_ptr;

    m_buffer_ptr = new MutatorScatterBuffer(m_props_ptr, m_comm, &m_table_identifier, m_schema_ptr, m_range_locator_ptr);
    m_memory_used = 0;
  }

}


void Mutator::flush() {

  if (m_prev_buffer_ptr)
    wait_for_previous_buffer();

  /**
   * If there are buffered updates, send them and wait for completion
   */
  if (m_memory_used > 0) {
    m_buffer_ptr->send();
    m_prev_buffer_ptr = m_buffer_ptr;
    wait_for_previous_buffer();
  }

  m_buffer_ptr->reset();
  m_prev_buffer_ptr = 0;
}

void Mutator::wait_for_previous_buffer() {
  int error;
  MutatorScatterBuffer *redo_buffer = 0;
  int wait_time = 1;

  while ((error = m_prev_buffer_ptr->wait_for_completion()) != Error::OK && wait_time < 16) {

    // wait a bit
    poll(0, 0, wait_time*1000);
    wait_time += 2;

    /**
     * Make several attempts to create redo buffer
     */
    if ((redo_buffer = m_prev_buffer_ptr->create_redo_buffer()) == 0)
      continue;

    m_prev_buffer_ptr = redo_buffer;

    /**
     * Re-send failed sends
     */
    m_prev_buffer_ptr->send();

  }

  if (error != Error::OK) {
    LOG_VA_ERROR("Problem resending failed updates (table=%s) - %s", m_table_name.c_str(), Error::get_text(error));
    throw Exception(error, (std::string)"Problem resending failed updates (table=" + m_table_name.c_str() + ")");
  }
  
}



void Mutator::sanity_check_key(KeySpec &key) {
  const char *row = (const char *)key.row;
  const char *column_qualifier = (const char *)key.column_qualifier;

  /**
   * Sanity check the row key
   */
  if (key.row_len == 0)
    throw Exception(Error::BAD_KEY, "Invalid row key - cannot be zero length");

  if (row[key.row_len] != 0)
    throw Exception(Error::BAD_KEY, "Invalid row key - must be followed by a '\0' character");

  if (strlen(row) != key.row_len)
    throw Exception(Error::BAD_KEY, (std::string)"Invalid row key - '\0' character not allowed (offset=" + (uint32_t)strlen(row) + ")");

  if (row[0] == (char)0xff && row[1] == (char)0xff)
    throw Exception(Error::BAD_KEY, "Invalid row key - cannot start with character sequence 0xff 0xff");

  /**
   * Sanity check the column qualifier
   */
  if (key.column_qualifier_len > 0) {
    if (column_qualifier[key.column_qualifier_len] != 0)
      throw Exception(Error::BAD_KEY, "Invalid column qualifier - must be followed by a '\0' character");
    if (strlen(column_qualifier) != key.column_qualifier_len)
      throw Exception(Error::BAD_KEY, (std::string)"Invalid column qualifier - '\0' character not allowed (offset=" + (uint32_t)strlen(column_qualifier) + ")");
  }
}
