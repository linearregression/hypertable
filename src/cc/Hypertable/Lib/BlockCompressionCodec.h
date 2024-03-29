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

#ifndef HYPERTABLE_BLOCKCOMPRESSIONCODEC_H
#define HYPERTABLE_BLOCKCOMPRESSIONCODEC_H

#include <string>

#include "Common/Error.h"

#include "BlockCompressionHeader.h"

namespace Hypertable {

  class DynamicBuffer;

  /**
   * Abstract base class for block compression codecs.  
   */
  class BlockCompressionCodec {
  public:
    virtual ~BlockCompressionCodec() { return; }

    virtual int deflate(const DynamicBuffer &input, DynamicBuffer &output, BlockCompressionHeader *header, size_t reserve=0) = 0;
    virtual int inflate(const DynamicBuffer &input, DynamicBuffer &output, BlockCompressionHeader *header) = 0;
    virtual int set_args(std::string args) { return Error::OK; }

    static const int ZLIB;
    static const int LZO;
    static const int BMZ;
  };

}

#endif // HYPERTABLE_BLOCKCOMPRESSIONCODEC_H

