/**
 * Copyright 2007 Doug Judd (Zvents, Inc.)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ScannerMap.h"

using namespace hypertable;

atomic_t ScannerMap::msNextId = ATOMIC_INIT(0);

/**
 *
 */
uint32_t ScannerMap::Put(CellListScannerPtr &scannerPtr, RangePtr &rangePtr) {
  boost::mutex::scoped_lock lock(mMutex);
  ScanInfoT scanInfo;
  scanInfo.scannerPtr = scannerPtr;
  scanInfo.rangePtr = rangePtr;
  uint32_t id = atomic_inc_return(&msNextId);
  mScannerMap[id] = scanInfo;
  return id;
}



/**
 *
 */
bool ScannerMap::Get(uint32_t id, CellListScannerPtr &scannerPtr, RangePtr &rangePtr) {
  boost::mutex::scoped_lock lock(mMutex);
  CellListScannerMapT::iterator iter = mScannerMap.find(id);
  if (iter == mScannerMap.end())
    return false;
  scannerPtr = (*iter).second.scannerPtr;
  rangePtr = (*iter).second.rangePtr;
  return true;
}



/**
 * 
 */
bool ScannerMap::Remove(uint32_t id) {
  boost::mutex::scoped_lock lock(mMutex);
  return (mScannerMap.erase(id) == 0) ? false : true;
}
