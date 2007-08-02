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

package org.hypertable.HdfsBroker;

import java.nio.ByteBuffer;
import org.hypertable.AsyncComm.Comm;
import org.hypertable.AsyncComm.CommBuf;
import org.hypertable.AsyncComm.Event;
import org.hypertable.AsyncComm.ResponseCallback;
import org.hypertable.Common.Error;

public class ResponseCallbackRead extends ResponseCallback {

    ResponseCallbackRead(Comm comm, Event event) {
	super(comm, event);
    }

    int response(long offset, int nread, byte [] data) {
	CommBuf cbuf = new CommBuf(mHeaderBuilder.HeaderLength() + 16);
	cbuf.PrependInt(nread);
	cbuf.PrependLong(offset);
	cbuf.PrependInt(Error.OK);
	
	if (nread > 0) {
	    cbuf.ext = ByteBuffer.allocateDirect(nread);
	    cbuf.ext.put(data, 0, nread);
	    cbuf.ext.flip();
	}

	// Encapsulate with Comm message response header
	mHeaderBuilder.LoadFromMessage(mEvent.msg);
	mHeaderBuilder.Encapsulate(cbuf);

	return mComm.SendResponse(mEvent.addr, cbuf);
    }
}

