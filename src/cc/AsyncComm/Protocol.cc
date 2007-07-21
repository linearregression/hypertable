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


#include <cassert>
#include <iostream>

#include "Common/Error.h"
#include "Common/Logger.h"

#include "CommBuf.h"
#include "Event.h"
#include "Protocol.h"

using namespace hypertable;
using namespace std;


/**
 *
 */
int32_t Protocol::ResponseCode(Event *event) {
  if (event->messageLen < sizeof(int32_t))
    return Error::RESPONSE_TRUNCATED;
  int32_t error;
  memcpy(&error, event->message, sizeof(int32_t));
  return error;
}



/**
 *
 */
std::string Protocol::StringFormatMessage(Event *event) {
  int error = 0;
  uint16_t command;
  uint8_t *msgPtr = event->message;
  size_t len = event->messageLen;

  if (event == 0)
    return (std::string)"NULL event";

  if (event->type != Event::MESSAGE)
    return event->toString();

  if (len < sizeof(int32_t))
    return (std::string)"Message Truncated";

  memcpy(&error, msgPtr, sizeof(int32_t));
  msgPtr += sizeof(int32_t);
  len -= sizeof(int32_t);

  if (len < sizeof(int16_t))
    return (std::string)Error::GetText(error) + " - truncated";

  memcpy(&command, msgPtr, sizeof(int16_t));
  msgPtr += sizeof(int16_t);
  len -= sizeof(int16_t);

  if (error == Error::OK)
    return (std::string)"command='" + CommandText(command) + "'"; 
  else {
    const char *str;

    if (CommBuf::DecodeString(msgPtr, len, &str) == 0)
      return (std::string)Error::GetText(error) + " command='" + CommandText(command) + "' - truncated";

    return (std::string)"ERROR command='" + CommandText(command) + "' [" + Error::GetText(error) + "] " + str;
  }
}



/**
 *
 */
CommBuf *Protocol::CreateErrorMessage(uint16_t command, int error, const char *msg, int extraSpace) {
  CommBuf *cbuf = new CommBuf(extraSpace + 6 + CommBuf::EncodedLength(msg));
  cbuf->PrependString(msg);
  cbuf->PrependShort(command);
  cbuf->PrependInt(error);
  return cbuf;
}
