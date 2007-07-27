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


#ifndef HYPERTABLE_IOHANDLERACCEPT_H
#define HYPERTABLE_IOHANDLERACCEPT_H

#include <boost/shared_ptr.hpp>

#include "IOHandler.h"
#include "ConnectionHandlerFactory.h"

extern "C" {
#include <stdint.h>
}

namespace hypertable {

  /**
   *
   */
  class IOHandlerAccept : public IOHandler {

  public:

    IOHandlerAccept(int sd, struct sockaddr_in &addr, CallbackHandler *cbh, HandlerMap &hmap, ConnectionHandlerFactory *hfactory) : IOHandler(sd, addr, cbh, hmap) {
      mHandlerFactory = hfactory;
      return;
    }

    virtual ~IOHandlerAccept() {
      return;
    }

#if defined(__APPLE__)
    virtual bool HandleEvent(struct kevent *event);
#elif defined(__linux__)
    virtual bool HandleEvent(struct epoll_event *event);
#else
    ImplementMe;
#endif

    bool HandleIncomingConnection();

  private:
    ConnectionHandlerFactory *mHandlerFactory;
  };

  typedef boost::shared_ptr<IOHandlerAccept> IOHandlerAcceptPtr;


}


#endif // HYPERTABLE_IOHANDLERACCEPT_H

