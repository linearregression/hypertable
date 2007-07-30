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

import org.apache.hadoop.fs.Path;

import java.nio.ByteBuffer;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.ProtocolException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Logger;
import java.util.logging.Level;

import org.hypertable.AsyncComm.Comm;
import org.hypertable.AsyncComm.CommBuf;
import org.hypertable.AsyncComm.Event;
import org.hypertable.AsyncComm.Message;
import org.hypertable.AsyncComm.HeaderBuilder;

import org.hypertable.Common.Error;


public class RequestRead extends Request {

    public RequestRead(OpenFileMap ofmap, Event event) throws ProtocolException {
	super(ofmap, event);

	if (event.msg.buf.remaining() < 8)
	    throw new ProtocolException("Truncated message");

	mFileId = event.msg.buf.getInt();
	mAmount = event.msg.buf.getInt();

	mOpenFileData = mOpenFileMap.Get(mFileId);
    }

    public void run() {
	int error = Error.HDFSBROKER_IO_ERROR;
	CommBuf cbuf = null;

	try {

	    /**
	    if (Global.verbose)
		log.info("Read request handle=" + mFileId + " amount=" + mAmount);
	    */

	    if (mOpenFileData == null) {
		error = Error.HDFSBROKER_BAD_FILE_HANDLE;
		throw new IOException("Invalid file handle " + mFileId);
	    }
	    
	    if (mOpenFileData.is == null)
		throw new IOException("File handle " + mFileId + " not open for reading");

	    long offset = mOpenFileData.is.getPos();

	    byte [] data = new byte [ mAmount ];

	    int nread = mOpenFileData.is.read(data, 0, data.length);
	    
	    cbuf = new CommBuf(mOpenFileData.hbuilder.HeaderLength() + 22);
	    cbuf.PrependInt(nread);
	    cbuf.PrependLong(offset);
	    cbuf.PrependInt(mFileId);
	    cbuf.PrependShort(Protocol.COMMAND_READ);
	    cbuf.PrependInt(Error.OK);

	    if (nread > 0) {
		cbuf.ext = ByteBuffer.allocateDirect(nread);
		cbuf.ext.put(data, 0, nread);
		cbuf.ext.flip();
	    }

	    // Encapsulate with Comm message response header
	    mOpenFileData.hbuilder.LoadFromMessage(mEvent.msg);
	    mOpenFileData.hbuilder.Encapsulate(cbuf);
	    
	    if ((error = Global.comm.SendResponse(mEvent.addr, cbuf)) != Error.OK)
		log.log(Level.SEVERE, "Comm.SendResponse returned " + Error.GetText(error));
	    return;
	}
	catch (IOException e) {
	    HeaderBuilder hbuilder = new HeaderBuilder();
	    e.printStackTrace();
	    cbuf = Global.protocol.CreateErrorMessage(Protocol.COMMAND_READ, error,
						      e.getMessage(), hbuilder.HeaderLength());

	    // Encapsulate with Comm message response header
	    hbuilder.LoadFromMessage(mEvent.msg);
	    hbuilder.Encapsulate(cbuf);

	    if ((error = Global.comm.SendResponse(mEvent.addr, cbuf)) != Error.OK)
		log.log(Level.SEVERE, "Comm.SendResponse returned " + Error.GetText(error));
	}
    }

    private int mAmount;

}
