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

#ifndef HYPERSPACE_RESPONSECALLBACKEXISTS_H
#define HYPERSPACE_RESPONSECALLBACKEXISTS_H

#include "Common/Error.h"

#include "AsyncComm/CommBuf.h"
#include "AsyncComm/ResponseCallback.h"

namespace Hyperspace {

  class ResponseCallbackExists : public Hypertable::ResponseCallback {
  public:
    ResponseCallbackExists(Hypertable::Comm *comm, Hypertable::EventPtr &eventPtr) : Hypertable::ResponseCallback(comm, eventPtr) { return; }
    int response(bool exists);
  };

}

#endif // HYPERSPACE_RESPONSECALLBACKEXISTS_H