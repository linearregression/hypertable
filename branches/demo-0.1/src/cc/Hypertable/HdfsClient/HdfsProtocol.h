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


#ifndef HDFS_PROTOCOL_H
#define HDFS_PROTOCOL_H

#include "AsyncComm/CommBuf.h"
#include "AsyncComm/Event.h"
#include "AsyncComm/MessageBuilder.h"
#include "AsyncComm/Protocol.h"
using namespace hypertable;

extern "C" {
#include <stdint.h>
#include <string.h>
}

namespace hypertable {

  class HdfsProtocol : public Protocol {

  public:

    CommBuf *CreateOpenRequest(const char *fname, uint32_t bufferSize=0);

    CommBuf *CreateCreateRequest(const char *fname, bool overwrite, int32_t bufferSize,
				 int32_t replication, int64_t blockSize);

    CommBuf *CreateCloseRequest(int32_t fd);

    CommBuf *CreateReadRequest(int32_t fd, uint32_t amount);

    CommBuf *CreateWriteRequest(int32_t fd, uint8_t *buf, uint32_t amount);

    CommBuf *CreateSeekRequest(int32_t fd, uint64_t offset);

    CommBuf *CreateRemoveRequest(const char *fname);

    CommBuf *CreateShutdownRequest(uint16_t flags);

    CommBuf *CreateLengthRequest(const char *fname);

    CommBuf *CreatePositionReadRequest(int32_t fd, uint64_t offset, uint32_t amount);

    CommBuf *CreateMkdirsRequest(const char *fname);

    virtual const char *CommandText(short command);

    /**
     * Request Headers
     */

    typedef struct {
      uint16_t command;
    } __attribute__((packed)) RequestHeaderT;

    typedef struct {
      RequestHeaderT hdr;
      uint32_t       bufferSize;
    } __attribute__((packed)) RequestHeaderOpenT;

    typedef struct {
      RequestHeaderT hdr;
      int16_t        overwrite;
      int32_t        replication;
      int32_t        bufferSize;
      int64_t        blockSize;
    } __attribute__((packed)) RequestHeaderCreateT;

    typedef struct {
      RequestHeaderT hdr;
      int32_t        fd;
    } __attribute__((packed)) RequestHeaderCloseT;

    typedef struct {
      RequestHeaderT hdr;
      int32_t        fd;
      uint32_t       amount;
    } __attribute__((packed)) RequestHeaderReadT;

    typedef RequestHeaderReadT RequestHeaderWriteT;

    typedef struct {
      RequestHeaderT hdr;
      int32_t        fd;
      uint64_t       offset;
    } __attribute__((packed)) RequestHeaderSeekT;

    typedef  struct {
      RequestHeaderT hdr;
    } __attribute__((packed)) RequestHeaderRemoveT;

    typedef RequestHeaderRemoveT RequestHeaderLengthT;

    typedef RequestHeaderRemoveT RequestHeaderMkdirsT;

    typedef struct {
      RequestHeaderT hdr;
      int32_t        fd;
      uint64_t       offset;
      uint32_t       amount;
    } __attribute__((packed)) RequestHeaderPositionReadT;

    /**
     * Response Headers
     */

    typedef struct {
      int32_t  error;
      uint16_t command;
      int32_t  handle;
    } __attribute__((packed)) ResponseHeaderOpenT;

    typedef ResponseHeaderOpenT ResponseHeaderCreateT;

    typedef struct {
      int32_t  error;
      uint16_t command;
      int32_t  handle;
      uint64_t offset;
      int32_t  amount;
    } __attribute__((packed)) ResponseHeaderReadT;

    typedef ResponseHeaderReadT ResponseHeaderWriteT;

    typedef ResponseHeaderOpenT ResponseHeaderRemoveT;

    typedef ResponseHeaderOpenT ResponseHeaderMkdirsT;

    typedef struct {
      int32_t  error;
      uint16_t command;
      int64_t  length;
    } __attribute__((packed)) ResponseHeaderLengthT;

    static const uint16_t COMMAND_OPEN     = 0;
    static const uint16_t COMMAND_CREATE   = 1;
    static const uint16_t COMMAND_CLOSE    = 2;
    static const uint16_t COMMAND_READ     = 3;
    static const uint16_t COMMAND_WRITE    = 4;
    static const uint16_t COMMAND_SEEK     = 5;
    static const uint16_t COMMAND_REMOVE   = 6;
    static const uint16_t COMMAND_SHUTDOWN = 7;
    static const uint16_t COMMAND_LENGTH   = 8;
    static const uint16_t COMMAND_PREAD    = 9;
    static const uint16_t COMMAND_MKDIRS   = 10;
    static const uint16_t COMMAND_MAX      = 11;

    static const uint16_t SHUTDOWN_FLAG_IMMEDIATE = 0x0001;

    static const char * msCommandStrings[COMMAND_MAX];

  };

}

#endif // HDFS_PROTOCOL_H