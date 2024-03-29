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
#ifndef HYPERTABLE_ACCESSGROUP_H
#define HYPERTABLE_ACCESSGROUP_H

#include <queue>
#include <string>
#include <vector>

#include "Hypertable/Lib/Schema.h"
#include "Hypertable/Lib/Types.h"

#include "CellCache.h"
#include "CellStore.h"
#include "RangeInfo.h"

using namespace Hypertable;

namespace Hypertable {

  class AccessGroup : CellList {

  public:

    AccessGroup(TableIdentifierT &table_identifier, SchemaPtr &schemaPtr, Schema::AccessGroup *ag, RangeT *range);
    virtual ~AccessGroup();
    virtual int add(const ByteString32T *key, const ByteString32T *value);

    virtual const char *get_split_row();
    virtual void get_split_rows(std::vector<std::string> &split_rows, bool include_cache);
    virtual void get_cached_rows(std::vector<std::string> &rows);

    void lock() { boost::detail::thread::lock_ops<boost::mutex>::lock(m_mutex); m_cell_cache_ptr->lock(); }
    void unlock() { m_cell_cache_ptr->unlock(); boost::detail::thread::lock_ops<boost::mutex>::unlock(m_mutex); }

    CellListScanner *create_scanner(ScanContextPtr &scanContextPtr);

    bool include_in_scan(ScanContextPtr &scanContextPtr);
    uint64_t disk_usage();
    void add_cell_store(CellStorePtr &cellStorePtr, uint32_t id);
    bool needs_compaction();
    void run_compaction(uint64_t timestamp, bool major);
    uint64_t get_persist_timestamp() { return m_persist_timestamp; }

    const char *get_name() { return m_name.c_str(); }

    int shrink(std::string &new_start_row);

    void get_files(std::string &text);

  private:
    boost::mutex         m_mutex;
    TableIdentifierT     m_table_identifier;
    SchemaPtr            m_schema_ptr;
    std::set<uint8_t>    m_column_families;
    std::string          m_name;
    std::string          m_table_name;
    std::string          m_start_row;
    std::string          m_end_row;
    std::vector<CellStorePtr> m_stores;
    CellCachePtr         m_cell_cache_ptr;
    uint32_t             m_next_table_id;
    uint64_t             m_persist_timestamp;
    uint64_t             m_disk_usage;
    uint32_t             m_blocksize;
    float                m_compression_ratio;
    std::string          m_compressor;
  };

}

#endif // HYPERTABLE_ACCESSGROUP_H
