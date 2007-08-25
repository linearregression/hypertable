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

#ifndef HYPERTABLE_REQUESTHANDLERSEEK_H
#define HYPERTABLE_REQUESTHANDLERSEEK_H

#include "Common/Runnable.h"

#include "AsyncComm/ApplicationHandler.h"
#include "AsyncComm/Comm.h"
#include "AsyncComm/Event.h"

#include "Broker.h"

using namespace hypertable;

namespace hypertable {

  namespace DfsBroker {

    class RequestHandlerSeek : public ApplicationHandler {
    public:
      RequestHandlerSeek(Comm *comm, Broker *broker, EventPtr &eventPtr) : ApplicationHandler(eventPtr), mComm(comm), mBroker(broker) {
	return;
      }

      virtual void run();

    private:
      Comm   *mComm;
      Broker *mBroker;
    };

  }

}

#endif // HYPERTABLE_REQUESTHANDLERSEEK_H
