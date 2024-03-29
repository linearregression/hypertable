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

package org.hypertable.Hyperspace;

import java.nio.ByteBuffer;
import org.hypertable.AsyncComm.Comm;
import org.hypertable.AsyncComm.CommBuf;
import org.hypertable.AsyncComm.Event;
import org.hypertable.AsyncComm.ResponseCallback;
import org.hypertable.AsyncComm.Serialization;
import org.hypertable.Common.Error;

public class ResponseCallbackAttrGet extends ResponseCallback {

    ResponseCallbackAttrGet(Comm comm, Event event) {
	super(comm, event);
    }

    int response(String attrValue) {
	mHeaderBuilder.InitializeFromRequest(mEvent.msg);
	CommBuf cbuf = new CommBuf(mHeaderBuilder, 4 + Serialization.EncodedLengthString(attrValue));
	cbuf.AppendInt(Error.OK);
	cbuf.AppendString(attrValue);
	return mComm.SendResponse(mEvent.addr, cbuf);
    }
}

