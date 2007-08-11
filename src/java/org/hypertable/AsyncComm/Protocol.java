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


package org.hypertable.AsyncComm;

import org.hypertable.Common.Error;

public abstract class Protocol {

    public static int ResponseCode(Event event) {
	event.msg.RewindToProtocolHeader();
	if (event.msg.buf.remaining() < 4)
	    return Error.RESPONSE_TRUNCATED;
	return event.msg.buf.getInt();
    }

    public String StringFormatMessage(Event event) {
	event.msg.RewindToProtocolHeader();
	if (event.type != Event.Type.MESSAGE)
	    return event.toString();

	if (event.msg.buf.remaining() < 4)
	    return "Message Truncated";

	int error = event.msg.buf.getInt();

	if (event.msg.buf.remaining() < 2)
	    return Error.GetText(error) + " - truncated";

	short command = event.msg.buf.getShort();

	if (error == Error.OK)
	    return "command='" + CommandText(command) + "'"; 
	else {
	    String str = CommBuf.DecodeString(event.msg.buf);

	    if (str == null)
		return Error.GetText(error) + " command='" + CommandText(command) + "' - truncated";

	    return "ERROR command='" + CommandText(command) + "' [" + Error.GetText(error) + "] " + str;
	}
    }

    public abstract String CommandText(short command);

    public static CommBuf CreateErrorMessage(short command, int error, String msg, int extraSpace) {
	CommBuf cbuf = new CommBuf(extraSpace + 6 + CommBuf.EncodedLength(msg));
	cbuf.PrependString(msg);
	cbuf.PrependShort(command);
	cbuf.PrependInt(error);
	return cbuf;
    }

}
