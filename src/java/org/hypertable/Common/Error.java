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


package org.hypertable.Common;

import java.util.HashMap;

public class Error {

    static public final int OK                 = 0;
    static public final int PROTOCOL_ERROR     = 1;
    static public final int REQUEST_TRUNCATED  = 2;
    static public final int RESPONSE_TRUNCATED = 3;
    static public final int REQUEST_TIMEOUT    = 4;
    static public final int LOCAL_IO_ERROR     = 5;

    static public final int COMM_NOT_CONNECTED      = 0x00010001;
    static public final int COMM_BROKEN_CONNECTION  = 0x00010002;
    static public final int COMM_CONNECT_ERROR      = 0x00010003;
    static public final int COMM_ALREADY_CONNECTED  = 0x00010004;
    static public final int COMM_REQUEST_TIMEOUT    = 0x00010005;
    static public final int COMM_SEND_ERROR         = 0x00010006;
    static public final int COMM_RECEIVE_ERROR      = 0x00010007;
    static public final int COMM_POLL_ERROR         = 0x00010008;

    static public final int DFSBROKER_BAD_FILE_HANDLE   = 0x00020001;
    static public final int DFSBROKER_IO_ERROR          = 0x00020002;
    static public final int DFSBROKER_FILE_NOT_FOUND    = 0x00020003;
    static public final int DFSBROKER_BAD_FILENAME      = 0x00020004;
    static public final int DFSBROKER_PERMISSION_DENIED = 0x00020005;
    static public final int DFSBROKER_INVALID_ARGUMENT  = 0x00020006;

    static public final int HYPERSPACE_IO_ERROR       = 0x00030001;
    static public final int HYPERSPACE_CREATE_FAILED  = 0x00030002;
    static public final int HYPERSPACE_FILE_NOT_FOUND = 0x00030003;
    static public final int HYPERSPACE_ATTR_NOT_FOUND = 0x00030004;
    static public final int HYPERSPACE_DELETE_ERROR   = 0x00030005;

    static public final int MASTER_TABLE_EXISTS  = 0x00040001;
    static public final int MASTER_BAD_SCHEMA    = 0x00040002;

    static public final int RANGESERVER_GENERATION_MISMATCH  = 0x00050001;
    static public final int RANGESERVER_RANGE_ALREADY_LOADED = 0x00050002;
    static public final int RANGESERVER_RANGE_MISMATCH       = 0x00050003;
    static public final int RANGESERVER_NONEXISTENT_RANGE    = 0x00050004;
    static public final int RANGESERVER_PARTIAL_UPDATE       = 0x00050005;
    static public final int RANGESERVER_RANGE_NOT_FOUND      = 0x00050006;
    static public final int RANGESERVER_INVALID_SCANNER_ID   = 0x00050007;
    static public final int RANGESERVER_SCHEMA_PARSE_ERROR   = 0x00050008;
    static public final int RANGESERVER_SCHEMA_INVALID_CFID  = 0x00050009;
    static public final int RANGESERVER_INVALID_COLUMNFAMILY = 0x0005000A;

    static public String GetText(int lcode) {
	return mTextMap.get(lcode);
    }

    static private HashMap<Integer, String> mTextMap = new HashMap<Integer, String>();

    static {
	mTextMap.put(OK,                         "HYPERTABLE ok");
	mTextMap.put(PROTOCOL_ERROR,             "HYPERTABLE protocol error");
	mTextMap.put(REQUEST_TRUNCATED,          "HYPERTABLE request truncated");
	mTextMap.put(RESPONSE_TRUNCATED,         "HYPERTABLE response truncated");
	mTextMap.put(REQUEST_TIMEOUT,            "HYPERTABLE request timeout");
	mTextMap.put(LOCAL_IO_ERROR,             "HYPERTABLE local i/o error");
	mTextMap.put(COMM_NOT_CONNECTED,         "COMM not connected");
	mTextMap.put(COMM_BROKEN_CONNECTION,     "COMM broken connection");
	mTextMap.put(COMM_CONNECT_ERROR,         "COMM connect error");
	mTextMap.put(COMM_ALREADY_CONNECTED,     "COMM already connected");
	mTextMap.put(COMM_REQUEST_TIMEOUT,       "COMM request timeout");
	mTextMap.put(COMM_SEND_ERROR,            "COMM send error");
	mTextMap.put(COMM_RECEIVE_ERROR,         "COMM receive error");
	mTextMap.put(COMM_POLL_ERROR,            "COMM poll error");
	mTextMap.put(DFSBROKER_BAD_FILE_HANDLE,  "DFS BROKER bad file handle");
	mTextMap.put(DFSBROKER_IO_ERROR,         "DFS BROKER i/o error");
	mTextMap.put(DFSBROKER_FILE_NOT_FOUND,   "DFS BROKER file not found");
	mTextMap.put(DFSBROKER_BAD_FILENAME,     "DFS BROKER bad filename");
	mTextMap.put(DFSBROKER_PERMISSION_DENIED,"DFS BROKER permission denied");
	mTextMap.put(DFSBROKER_INVALID_ARGUMENT, "DFS BROKER invalid argument");
	mTextMap.put(HYPERSPACE_IO_ERROR,        "HYPERTABLE FS i/o error");
	mTextMap.put(HYPERSPACE_CREATE_FAILED,   "HYPERTABLE FS create failed");
	mTextMap.put(HYPERSPACE_FILE_NOT_FOUND,  "HYPERTABLE FS file not found");
	mTextMap.put(HYPERSPACE_ATTR_NOT_FOUND,  "HYPERTABLE FS attribute not found");
	mTextMap.put(MASTER_TABLE_EXISTS,        "HYPERTABLE MASTER table exists");
	mTextMap.put(MASTER_TABLE_EXISTS,        "HYPERTABLE MASTER bad schema");
	mTextMap.put(RANGESERVER_GENERATION_MISMATCH,  "RANGE SERVER generation mismatch");
	mTextMap.put(RANGESERVER_RANGE_ALREADY_LOADED, "RANGE SERVER range already loaded");
	mTextMap.put(RANGESERVER_RANGE_MISMATCH,       "RANGE SERVER range mismatch");
	mTextMap.put(RANGESERVER_NONEXISTENT_RANGE,    "RANGE SERVER non-existent range");
	mTextMap.put(RANGESERVER_PARTIAL_UPDATE,       "RANGE SERVER partial update");
	mTextMap.put(RANGESERVER_RANGE_NOT_FOUND,      "RANGE SERVER range not found");
	mTextMap.put(RANGESERVER_INVALID_SCANNER_ID,   "RANGE SERVER invalid scanner id");
	mTextMap.put(RANGESERVER_SCHEMA_PARSE_ERROR,   "RANGE SERVER schema parse error");
	mTextMap.put(RANGESERVER_SCHEMA_INVALID_CFID,  "RANGE SERVER invalid column family id");
	mTextMap.put(RANGESERVER_INVALID_COLUMNFAMILY, "RANGE SERVER invalid column family");
    }
}