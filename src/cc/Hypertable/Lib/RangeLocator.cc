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

#include "Hyperspace/Session.h"

#include "RootFileHandler.h"
#include "RangeLocator.h"


/**
 * 
 */
RangeLocator::RangeLocator(Hyperspace::Session *hyperspace) : mHyperspace(hyperspace), mCache(1000), mRootStale(true) {
  int error;

  mRootHandlerPtr = new RootFileHandler(this);

  if ((error = mHyperspace->Open("/hypertable/root", OPEN_FLAG_READ, mRootHandlerPtr, &mRootFileHandle)) != Error::OK) {
    LOG_VA_ERROR("Unable to open Hyperspace file '/hypertable/root' (%s)", Error::GetText(error));
    throw Exception(error);
  }

}

RangeLocator::~RangeLocator() {
  mHyperspace->Close(mRootFileHandle);
}



/**
 *
 */
int RangeLocator::Find(uint32_t tableId, const char *rowKey, const char **serverIdPtr) {
  int error;

  if (mRootStale) {
    if ((error = ReadRootLocation()) != Error::OK)
      return error;
  }

  if (mCache.Lookup(tableId, rowKey, serverIdPtr))
    return Error::OK;

  // fix me !!!!

  if (tableId == 0) {

  }

  return Error::OK;
}


/**
 * 
 */
int RangeLocator::ReadRootLocation() {
  int error;
  DynamicBuffer value(0);
  std::string addrStr;

  if ((error = mHyperspace->AttrGet(mRootFileHandle, "location", value)) != Error::OK) {
    LOG_VA_ERROR("Problem reading 'address' attribute of Hyperspace file /hypertable/root - %s", Error::GetText(error));
    return error;
  }

  mCache.Insert(0, 0, "1:", (const char *)value.buf);
  
}
