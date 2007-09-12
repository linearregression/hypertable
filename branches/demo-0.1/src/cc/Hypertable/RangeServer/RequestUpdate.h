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

#ifndef HYPERTABLE_REQUESTUPDATE_H
#define HYPERTABLE_REQUESTUPDATE_H

#include "Common/Runnable.h"
#include "AsyncComm/CommBuf.h"
#include "AsyncComm/Event.h"

using namespace hypertable;

namespace hypertable {

  class RequestUpdate : public Runnable {
  public:
    RequestUpdate(Event &event) : mEvent(event) {
      return;
    }

    virtual void run();

  private:

    void SendBackInvalidUpdates(const uint8_t *modData, int32_t modLen);

    Event mEvent;
  };

}

#endif // HYPERTABLE_REQUESTUPDATE_H