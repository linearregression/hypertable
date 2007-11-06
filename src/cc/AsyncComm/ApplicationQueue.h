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

#ifndef HYPERTABLE_APPLICATIONQUEUE_H
#define HYPERTABLE_APPLICATIONQUEUE_H

#include <cassert>
#include <list>
#include <ext/hash_map>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "Common/ReferenceCount.h"
#include "Common/StringExt.h"

#include "ApplicationHandler.h"

namespace hypertable {

  class ApplicationQueue : public ReferenceCount {

    class UsageRec {
    public:
      UsageRec() : threadGroup(0), running(false), outstanding(1) { return; }
      uint64_t threadGroup;
      bool     running;
      int      outstanding;
    };

    typedef __gnu_cxx::hash_map<uint64_t, UsageRec *> UsageRecMapT;

    class WorkRec {
    public:
      WorkRec(ApplicationHandler *ah) : handler(ah), usage(0) { return; }
      ~WorkRec() { delete handler; }
      ApplicationHandler   *handler;
      UsageRec             *usage;
    };

    class ApplicationQueueState {
    public:
      ApplicationQueueState() : queue(), usageMap(), queueMutex(), usageMutex(), cond(), shutdown(false) { return; }
      list<WorkRec *>     queue;
      UsageRecMapT        usageMap;
      boost::mutex        queueMutex;
      boost::mutex        usageMutex;
      boost::condition    cond;
      bool                shutdown;
    };

    class Worker {

    public:

      Worker(ApplicationQueueState &qstate) : m_state(qstate) {
	return;
      }

      void operator()() {
	WorkRec *rec = 0;
	list<WorkRec *>::iterator iter;

	while (true) {

	  {  // !!! maybe ditch this block specifier
	    boost::mutex::scoped_lock lock(m_state.queueMutex);

	    while (m_state.queue.empty()) {
	      if (m_state.shutdown) {
		cerr << "shutdown!!!" << endl;
		return;
	      }
	      m_state.cond.wait(lock);
	    }

	    {
	      boost::mutex::scoped_lock ulock(m_state.usageMutex);

	      for (iter = m_state.queue.begin(); iter != m_state.queue.end(); iter++) {
		rec = (*iter);
		if (rec->usage == 0 || !rec->usage->running) {
		  if (rec->usage)
		    rec->usage->running = true;
		  m_state.queue.erase(iter);
		  break;
		}
		rec = 0;
	      }
	    }
	  }
		    
	  if (rec) {
	    rec->handler->run();
	    if (rec->usage) {
	      boost::mutex::scoped_lock ulock(m_state.usageMutex);
	      rec->usage->running = false;
	      rec->usage->outstanding--;
	      if (rec->usage->outstanding == 0) {
		m_state.usageMap.erase(rec->usage->threadGroup);
		delete rec->usage;
	      }
	    }
	    delete rec;
	  }
	}

	cerr << "thread exit" << endl;
      }

    private:
      ApplicationQueueState &m_state;
    };

    ApplicationQueueState  m_state;
    boost::thread_group    m_threads;

  public:

    ApplicationQueue(int WorkerCount) {
      Worker Worker(m_state);
      assert (WorkerCount > 0);
      for (int i=0; i<WorkerCount; ++i)
	m_threads.create_thread(Worker);
      //threads
    }

    void shutdown() {
      m_state.shutdown = true;
      m_state.cond.notify_all();
    }

    void join() {
      m_threads.join_all();
    }

    void add(ApplicationHandler *appHandler) {
      UsageRecMapT::iterator uiter;
      uint64_t threadGroup = appHandler->get_thread_group();
      WorkRec *rec = new WorkRec(appHandler);
      rec->usage = 0;

      if (threadGroup != 0) {
	boost::mutex::scoped_lock ulock(m_state.usageMutex);
	if ((uiter = m_state.usageMap.find(threadGroup)) != m_state.usageMap.end()) {
	  rec->usage = (*uiter).second;
	  rec->usage->outstanding++;
	}
	else {
	  rec->usage = new UsageRec();
	  rec->usage->threadGroup = threadGroup;
	  m_state.usageMap[threadGroup] = rec->usage;
	}
      }

      {
	boost::mutex::scoped_lock lock(m_state.queueMutex);
	m_state.queue.push_back(rec);
	m_state.cond.notify_one();
      }
    }
  };
  typedef boost::intrusive_ptr<ApplicationQueue> ApplicationQueuePtr;

}


#endif // HYPERTABLE_APPLICATIONQUEUE_H