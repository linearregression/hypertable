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

#ifndef HYPERTABLE_ERROR_H
#define HYPERTABLE_ERROR_H

#include <ext/hash_map>
#include <string>
using namespace std;

namespace hypertable {

  class Error {

  public:

    static const int OK                 = 0;
    static const int PROTOCOL_ERROR     = 1;
    static const int REQUEST_TRUNCATED  = 2;
    static const int RESPONSE_TRUNCATED = 3;
    static const int REQUEST_TIMEOUT    = 4;
    static const int LOCAL_IO_ERROR     = 5;

    static const int COMM_NOT_CONNECTED     = 0x00010001;
    static const int COMM_BROKEN_CONNECTION = 0x00010002;
    static const int COMM_CONNECT_ERROR     = 0x00010003;
    static const int COMM_ALREADY_CONNECTED = 0x00010004;
    static const int COMM_REQUEST_TIMEOUT   = 0x00010005;

    static const int HDFSBROKER_BAD_FILE_HANDLE = 0x00020001;
    static const int HDFSBROKER_IO_ERROR        = 0x00020002;
    static const int HDFSBROKER_FILE_NOT_FOUND  = 0x00020003;

    static const int HYPERTABLEFS_IO_ERROR       = 0x00030001;
    static const int HYPERTABLEFS_CREATE_FAILED  = 0x00030002;
    static const int HYPERTABLEFS_FILE_NOT_FOUND = 0x00030003;
    static const int HYPERTABLEFS_ATTR_NOT_FOUND = 0x00030004;

    static const int BTMASTER_TABLE_EXISTS = 0x00040001;

    static const int RANGESERVER_GENERATION_MISMATCH  = 0x00050001;
    static const int RANGESERVER_RANGE_ALREADY_LOADED = 0x00050002;
    static const int RANGESERVER_RANGE_MISMATCH       = 0x00050003;
    static const int RANGESERVER_NONEXISTENT_RANGE    = 0x00050004;
    static const int RANGESERVER_PARTIAL_UPDATE       = 0x00050005;
    static const int RANGESERVER_RANGE_NOT_FOUND      = 0x00050006;
    static const int RANGESERVER_INVALID_SCANNER_ID   = 0x00050007;
    static const int RANGESERVER_SCHEMA_PARSE_ERROR   = 0x00050008;

    static const char *GetText(int error);

  };
}

#endif // HYPERTABLE_ERROR_H