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
#ifndef HYPERTABLE_MASTERPROTOCOL_H
#define HYPERTABLE_MASTERPROTOCOL_H

#include "AsyncComm/Protocol.h"

namespace hypertable {

  class MasterProtocol : public Protocol {

  public:

    static const short COMMAND_CREATE_TABLE = 0;
    static const short COMMAND_GET_SCHEMA   = 1;
    static const short COMMAND_MAX          = 2;

    static const char *mCommandStrings[];

    virtual const char *CommandText(short command);
  };

}

#endif // HYPERTABLE_MASTERPROTOCOL_H
