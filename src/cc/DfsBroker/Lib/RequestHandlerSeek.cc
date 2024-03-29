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

#include "Common/Error.h"
#include "Common/Logger.h"

#include "AsyncComm/ResponseCallback.h"
#include "AsyncComm/Serialization.h"

#include "RequestHandlerSeek.h"

using namespace Hypertable;
using namespace Hypertable::DfsBroker;

/**
 *
 */
void RequestHandlerSeek::run() {
  ResponseCallback cb(m_comm, m_event_ptr);
  uint32_t fd;
  uint64_t offset;
  size_t remaining = m_event_ptr->messageLen - sizeof(int16_t);
  uint8_t *msgPtr = m_event_ptr->message + sizeof(int16_t);

  if (remaining < 12)
    goto abort;

  // fd
  Serialization::decode_int(&msgPtr, &remaining, &fd);

  // offset
  Serialization::decode_long(&msgPtr, &remaining, &offset);

  m_broker->seek(&cb, fd, offset);

  return;

 abort:
  LOG_ERROR("Encoding problem with SEEK message");
  cb.error(Error::PROTOCOL_ERROR, "Encoding problem with SEEK message");
  return;
}
