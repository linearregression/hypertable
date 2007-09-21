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

#ifndef HYPERSPACE_CLIENTSESSIONSTATE_H
#define HYPERSPACE_CLIENTSESSIONSTATE_H

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "Common/ReferenceCount.h"

using namespace hypertable;

namespace Hyperspace {

  class ClientSessionState : public ReferenceCount {
  public:

    enum { EXPIRED, JEOPARDY, SAFE };

    ClientSessionState() : mState(EXPIRED) { return; }

    int Transition(int state) {
      boost::mutex::scoped_lock lock(mMutex);
      int oldState = mState;
      mState = state;
      if (mState == SAFE)
	mCond.notify_all();
      return oldState;
    }

    int Get() {
      boost::mutex::scoped_lock lock(mMutex);
      return mState;
    }

    bool WaitForSafe(long maxWaitSecs) {
      boost::mutex::scoped_lock lock(mMutex);
      boost::xtime dropTime, now;

      boost::xtime_get(&dropTime, boost::TIME_UTC);
      dropTime.sec += maxWaitSecs;

      while (mState != SAFE) {
	mCond.timed_wait(lock, dropTime);
	boost::xtime_get(&now, boost::TIME_UTC);
	if (xtime_cmp(now, dropTime) >= 0)
	  return false;
      }
      return true;
    }

  private:
    boost::mutex      mMutex;
    boost::condition  mCond;
    int               mState;
  };

  typedef boost::intrusive_ptr<ClientSessionState> ClientSessionStatePtr;

}

#endif // HYPERSPACE_CLIENTSESSIONSTATE_H
