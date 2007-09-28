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

#include <iostream>

#include "Common/DynamicBuffer.h"
#include "Common/Error.h"
#include "Common/Usage.h"

#include "CommandAttrGet.h"
#include "Global.h"
#include "NormalizePathname.h"

using namespace hypertable;
using namespace Hyperspace;
using namespace std;

const char *CommandAttrGet::msUsage[] = {
  "attrget <file> <name>",
  "  This command issues a ATTRGET request to Hyperspace.",
  (const char *)0
};


int CommandAttrGet::run() {
  uint64_t handle;
  int error;
  DynamicBuffer value(0);
  std::string normalName;

  if (mArgs.size() != 2) {
    cerr << "Wrong number of arguments.  Type 'help' for usage." << endl;
    return -1;
  }

  if (mArgs[0].second != "" || mArgs[1].second != "") {
    cerr << "Invalid character '=' in argument." << endl;
    return -1;
  }

  NormalizePathname(mArgs[0].first, normalName);

  Global::FileMapT::iterator iter = Global::fileMap.find(normalName);
  if (iter == Global::fileMap.end()) {
    LOG_VA_ERROR("Unable to find '%s' in open file map", normalName.c_str());
    return -1;
  }
  handle = (*iter).second;

  if ((error = mSession->AttrGet(handle, mArgs[1].first, value)) == Error::OK) {
    std::string valStr = std::string((const char *)value.buf, value.fill());
    cout << valStr << endl;
  }

  return error;
}
