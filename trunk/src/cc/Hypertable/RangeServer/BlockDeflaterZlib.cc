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

#include "Common/DynamicBuffer.h"
#include "Common/Logger.h"

#include "BlockDeflaterZlib.h"
#include "Constants.h"


/**
 *
 */
BlockDeflaterZlib::BlockDeflaterZlib() : BlockDeflater() {
  memset(&mStream, 0, sizeof(mStream));
  mStream.zalloc = Z_NULL;
  mStream.zfree = Z_NULL;
  mStream.opaque = Z_NULL;
  int ret = deflateInit(&mStream, Z_BEST_COMPRESSION);
  assert(ret == Z_OK);
}



/**
 *
 */
BlockDeflaterZlib::~BlockDeflaterZlib() {
  deflateEnd(&mStream);
}



/**
 *
 */
void BlockDeflaterZlib::deflate(hypertable::DynamicBuffer &inbuf, hypertable::DynamicBuffer &outbuf, const char magic[12], size_t reserve) {
  uint32_t avail_out = inbuf.fill() + 6 + (((inbuf.fill() / 16000) + 1 ) * 5);  // see http://www.zlib.net/zlib_tech.html

  outbuf.clear();
  outbuf.reserve( sizeof(Constants::BlockHeaderT) + avail_out + reserve );

  Constants::BlockHeaderT *header = (Constants::BlockHeaderT *)outbuf.buf;

  mStream.avail_in = inbuf.fill();
  mStream.next_in = inbuf.buf;

  mStream.avail_out = avail_out;
  mStream.next_out = outbuf.buf + sizeof(Constants::BlockHeaderT);

  int ret = ::deflate(&mStream, Z_FINISH); 
  assert(ret == Z_STREAM_END);

  memcpy(header->magic, magic, 12);
  header->zlength = (avail_out - mStream.avail_out);
  header->length = inbuf.fill();

  // compute checksum
  uint8_t *checkptr = (uint8_t *)header;
  header->checksum = 0;
  for (size_t i=0; i<sizeof(Constants::BlockHeaderT)-sizeof(int16_t); i++)
    header->checksum += checkptr[i];
  checkptr = outbuf.buf + sizeof(Constants::BlockHeaderT);
  for (size_t i=0; i<header->zlength; i++)
    header->checksum += checkptr[i];

  deflateReset(&mStream);

  outbuf.ptr = outbuf.buf + sizeof(Constants::BlockHeaderT) + header->zlength;
}


