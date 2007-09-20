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

#ifndef HYPERTABLE_EVENT_H
#define HYPERTABLE_EVENT_H

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>

extern "C" {
#include <stdint.h>
#include <netinet/in.h>
}

using namespace std;

#include "Header.h"

namespace hypertable {

  class Event {

  public:

    enum Type { CONNECTION_ESTABLISHED, DISCONNECT, MESSAGE, ERROR, TIMER };

    Event(Type ct, int cid, struct sockaddr_in &a, int err=0, Header::HeaderT *h=0) 
      : type(ct), addr(a), connId(cid), error(err), header(h) {
      if (h != 0) {
	message = ((uint8_t *)header) + header->headerLen;
	messageLen = header->totalLen - header->headerLen;
	if (header->gid != 0)
	  threadGroup = ((uint64_t)connId << 32) | header->gid;
	else
	  threadGroup = 0;
      }
      else {
	message = 0;
	messageLen = 0;
	threadGroup = 0;
      }
    }

    Event(Type ct, int err=0) : type(ct), error(err) {
      header = 0;
      message = 0;
      messageLen = 0;
      threadGroup = 0;
      connId = 0;
    }


    ~Event() {
      delete [] header;
    }

    Type                type;
    struct sockaddr_in  addr;
    uint16_t            receivePort;
    int                 connId;
    int                 error;
    Header::HeaderT    *header;
    uint8_t            *message;
    size_t              messageLen;
    uint64_t            threadGroup;

    std::string toString();
    void Display() { cerr << toString() << endl; }
  };

  typedef boost::shared_ptr<Event> EventPtr;

}

#endif // HYPERTABLE_EVENT_H