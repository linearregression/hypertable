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

#include "DispatchHandlerSynchronizer.h"
#include "Protocol.h"

using namespace hypertable;


/**
 *
 */
DispatchHandlerSynchronizer::DispatchHandlerSynchronizer() : mReceiveQueue(), mMutex(), mCond() {
  return;
}



/**
 *
 */
void DispatchHandlerSynchronizer::handle(EventPtr &eventPtr) {
  boost::mutex::scoped_lock lock(mMutex);
  mReceiveQueue.push(eventPtr);
  mCond.notify_one();    
}



/**
 * 
 */
bool DispatchHandlerSynchronizer::WaitForReply(EventPtr &eventPtr) {
  boost::mutex::scoped_lock lock(mMutex);

  while (mReceiveQueue.empty())
    mCond.wait(lock);

  eventPtr = mReceiveQueue.front();
  mReceiveQueue.pop();

  if (eventPtr->type == Event::MESSAGE && Protocol::ResponseCode(eventPtr.get()) == Error::OK)
    return true;

  return false;
}