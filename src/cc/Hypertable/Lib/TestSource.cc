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

#include <cerrno>
#include <cstring>

#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>

extern "C" {
#include <strings.h>
}

#include "Common/ByteOrder.h"
#include "Common/DynamicBuffer.h"

#include "Key.h"

#include "TestSource.h"

using namespace std;

bool TestSource::Next(ByteString32T **keyp, ByteString32T **valuep) {
  string line;
  boost::shared_array<char> linePtr;
  char *base, *ptr, *last;
  char *rowKey;
  char *column;
  char *value;
  uint64_t timestamp;

  while (getline(mFin, line)) {
    mCurLine++;

    boost::trim(line);

    linePtr.reset(new char [ strlen(line.c_str()) + 1 ]);
    base = linePtr.get();
    strcpy(base, line.c_str());

    if ((ptr = strtok_r(base, "\t", &last)) == 0) {
      cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
      continue;
    }

    if (!strcasecmp(ptr, "AUTO")) {
      timestamp = (uint64_t)-1;
    }
    else {
      timestamp = strtoll(ptr, 0, 0);
      if (timestamp == 0 && errno == EINVAL) {
	cerr << "Invalid timestamp (" << ptr << ") on line " << (mCurLine-1) << endl;
	continue;
      }
    }

    if ((rowKey = strtok_r(0, "\t", &last)) == 0) {
      cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
      continue;
    }

    if ((column = strtok_r(0, "\t", &last)) == 0) {
      cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
      continue;
    }

    if (!strcmp(column, "DELETE")) {
      if (!CreateRowDelete(rowKey, timestamp, keyp, valuep)) {
	cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
	continue;
      }
      return true;
    }

    if ((value = strtok_r(0, "\t", &last)) == 0) {
      cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
      continue;
    }

    if (!strcmp(value, "DELETE")) {
      if (!CreateColumnDelete(rowKey, column, timestamp, keyp, valuep)) {
	cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
	continue;
      }
      return true;
    }

    if (!CreateInsert(rowKey, column, timestamp, value, keyp, valuep)) {
      cerr << "Mal-formed input on line " << (mCurLine-1) << endl;
      continue;
    }

    return true;
  }

  return false;
}


bool TestSource::CreateRowDelete(const char *row, uint64_t timestamp, ByteString32T **keyp, ByteString32T **valuep) {
  int32_t keyLen = strlen(row) + 12;
  int32_t valueLen = 0;

  mKeyBuffer.clear();
  mKeyBuffer.ensure(sizeof(int32_t)+keyLen);

  mKeyBuffer.addNoCheck(&keyLen, sizeof(int32_t));
  mKeyBuffer.addNoCheck(row, strlen(row)+1);
  *mKeyBuffer.ptr++ = 0;
  *mKeyBuffer.ptr++ = 0;
  *mKeyBuffer.ptr++ = FLAG_DELETE_ROW;

  timestamp = ByteOrderSwapInt64(timestamp);
  timestamp = ~timestamp;
  mKeyBuffer.addNoCheck(&timestamp, sizeof(timestamp));
  *keyp = (ByteString32T *)mKeyBuffer.buf;

  mValueBuffer.clear();
  mValueBuffer.ensure(sizeof(int32_t));
  mValueBuffer.addNoCheck(&valueLen, sizeof(int32_t));  
  *valuep = (ByteString32T *)mValueBuffer.buf;
  return true;
}


bool TestSource::CreateColumnDelete(const char *row, const char *column, uint64_t timestamp, ByteString32T **keyp, ByteString32T **valuep) {
  int32_t keyLen = 0;
  int32_t valueLen = 0;
  string columnFamily;
  const char *qualifier;
  const char *ptr = strchr(column, ':');
  
  if (ptr == 0) {
    cerr << "Bad column family specifier (no family)" << endl;
    return false;
  }

  columnFamily = string(column, ptr-column);
  qualifier = ptr+1;

  Schema::ColumnFamily *cf = mSchema->GetColumnFamily(columnFamily);
  if (cf == 0) {
    cerr << "Column family '" << columnFamily << "' not found in schema" << endl;
    return false;
  }

  mKeyBuffer.clear();
  keyLen = strlen(row) + strlen(qualifier) + 12;
  mKeyBuffer.ensure(sizeof(int32_t)+keyLen);

  mKeyBuffer.addNoCheck(&keyLen, sizeof(int32_t));
  mKeyBuffer.addNoCheck(row, strlen(row)+1);
  *mKeyBuffer.ptr++ = cf->id;
  mKeyBuffer.addNoCheck(qualifier, strlen(qualifier)+1);
  *mKeyBuffer.ptr++ = FLAG_DELETE_CELL;

  timestamp = ByteOrderSwapInt64(timestamp);
  timestamp = ~timestamp;
  mKeyBuffer.addNoCheck(&timestamp, sizeof(timestamp));
  *keyp = (ByteString32T *)mKeyBuffer.buf;

  mValueBuffer.clear();
  mValueBuffer.ensure(sizeof(int32_t));
  mValueBuffer.addNoCheck(&valueLen, sizeof(int32_t));  
  *valuep = (ByteString32T *)mValueBuffer.buf;
  return true;
}


bool TestSource::CreateInsert(const char *row, const char *column, uint64_t timestamp, const char *value, ByteString32T **keyp, ByteString32T **valuep) {
  int32_t keyLen = 0;
  int32_t valueLen = strlen(value);
  string columnFamily;
  const char *qualifier;
  const char *ptr = strchr(column, ':');
  
  if (ptr == 0) {
    cerr << "Bad column family specifier (no family)" << endl;
    return false;
  }

  columnFamily = string(column, ptr-column);
  qualifier = ptr+1;

  Schema::ColumnFamily *cf = mSchema->GetColumnFamily(columnFamily);
  if (cf == 0) {
    cerr << "Column family '" << columnFamily << "' not found in schema" << endl;
    return false;
  }

  mKeyBuffer.clear();
  keyLen = strlen(row) + strlen(qualifier) + 12;
  mKeyBuffer.ensure(keyLen+sizeof(int32_t));

  mKeyBuffer.addNoCheck(&keyLen, sizeof(int32_t));
  mKeyBuffer.addNoCheck(row, strlen(row)+1);
  *mKeyBuffer.ptr++ = cf->id;
  mKeyBuffer.addNoCheck(qualifier, strlen(qualifier)+1);
  *mKeyBuffer.ptr++ = FLAG_INSERT;

  timestamp = ByteOrderSwapInt64(timestamp);
  timestamp = ~timestamp;
  mKeyBuffer.addNoCheck(&timestamp, sizeof(timestamp));
  *keyp = (ByteString32T *)mKeyBuffer.buf;

  mValueBuffer.clear();
  mValueBuffer.ensure(sizeof(int32_t) + valueLen);
  mValueBuffer.addNoCheck(&valueLen, sizeof(int32_t));  
  mValueBuffer.addNoCheck(value, valueLen);
  *valuep = (ByteString32T *)mValueBuffer.buf;
  return true;
}

