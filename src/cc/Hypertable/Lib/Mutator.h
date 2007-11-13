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

#ifndef HYPERTABLE_MUTATOR_H
#define HYPERTABLE_MUTATOR_H

#include "AsyncComm/ConnectionManager.h"

#include "Common/ReferenceCount.h"

#include "CellKey.h"
#include "MutationResult.h"
#include "Schema.h"

namespace Hypertable {

  class Mutator : public ReferenceCount {

  public:
    Mutator(ConnectionManagerPtr &connManagerPtr, SchemaPtr &schemaPtr);
    virtual ~Mutator() { return; }

    void set(CellKey &key, uint8_t *value, uint32_t valueLen);

    void flush(MutationResultPtr &resultPtr);

    uint64_t memory_used();

  private:
    ConnectionManagerPtr m_conn_manager_ptr;
    SchemaPtr m_schema_ptr;
  };
  typedef boost::intrusive_ptr<Mutator> MutatorPtr;

}

#endif // HYPERTABLE_MUTATOR_H
