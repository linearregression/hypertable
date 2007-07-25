/**
 * Copyright (C) 2007 Doug Judd (Zvents, Inc.)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef HYPERTABLE_RESPONSECALLBACKCREATESCANNER_H
#define HYPERTABLE_RESPONSECALLBACKCREATESCANNER_H

#include "Common/Error.h"

#include "AsyncComm/CommBuf.h"
#include "AsyncComm/ResponseCallback.h"

namespace hypertable {

  class ResponseCallbackCreateScanner : public ResponseCallback {
  public:
    ResponseCallbackCreateScanner(Comm *comm, EventPtr &eventPtr) : ResponseCallback(comm, eventPtr) { return; }
    int response(short moreFlag, int32_t id, ExtBufferT &ext);
  };

}


#endif // HYPERTABLE_RESPONSECALLBACKCREATESCANNER_H
