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
#include "Common/Usage.h"

#include "DfsBroker/Lib/DfsBrokerProtocol.h"

#include "CommandShutdown.h"

using namespace hypertable;

const char *CommandShutdown::msUsage[] = {
  "shutdown [now]",
  "",
  "  This command sends a shutdown request to the DfsBroker",
  "  server.  If the 'now' argument is given, the DfsBroker",
  "  will do an unclean shutdown by exiting immediately.  Otherwise",
  "  it will wait for all pending requests to complete before",
  "  shutting down.",
  (const char *)0
};


int CommandShutdown::run() {
  DispatchHandlerSynchronizer syncHandler;
  uint32_t msgId = 0;
  uint16_t flags = 0;
  EventPtr eventPtr;

  if (mArgs.size() > 0) {
    if (mArgs[0].first == "now")
      flags |= DfsBrokerProtocol::SHUTDOWN_FLAG_IMMEDIATE;
    else {
      Usage::Dump(msUsage);
      return -1;
    }
  }

  mClient->Shutdown(flags, &syncHandler, &msgId);

  syncHandler.WaitForReply(eventPtr);

  return Error::OK;
}


