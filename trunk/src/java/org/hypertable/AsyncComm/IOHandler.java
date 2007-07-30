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

package org.hypertable.AsyncComm;

import java.net.InetSocketAddress;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.AbstractSelectableChannel;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Logger;

public abstract class IOHandler {

    protected static final Logger log = Logger.getLogger("org.hypertable");

    public IOHandler(AbstractSelectableChannel chan, DispatchHandler dh, ConnectionMap cm, Event.Queue eq) {
	mChannel = chan;
	mDispatchHandler = dh;
	mConnMap = cm;
	mEventQueue = eq;
	mReactor = ReactorFactory.Get();
	mAddr = null;
	mInterest = 0;
    }

    public void DeliverEvent(Event event, DispatchHandler dh) {
	if (mEventQueue != null)
	    mEventQueue.Add(event);  // ??? , handler);
	else
	    dh.handle(event);
    }

    public void DeliverEvent(Event event) {
	if (mEventQueue != null)
	    mEventQueue.Add(event);  // ??? , handler);
	else if (mDispatchHandler != null)
	    mDispatchHandler.handle(event);
	else
	    log.info(event.toString());
    }


    public abstract void run(SelectionKey selkey);

    public void Register(Selector selector) {
	try {
	    if (mShuttingDown) {
		SelectionKey selkey = mChannel.register(selector, 0);
		selkey.cancel();
	    }
	    else {
		mChannel.register(selector, mInterest, this);
	    }
	}
	catch (ClosedChannelException e) {
	    e.printStackTrace();
	    if (mAddr != null)
		mConnMap.Remove(mAddr);
	}
    }

    public void Shutdown() {
	mShuttingDown = true;
	mReactor.AddToRegistrationQueue(this);
	mReactor.WakeUp();
    }

    public void SetInterest(int interest) {
	mInterest = interest;
	mReactor.AddToRegistrationQueue(this);
	mReactor.WakeUp();
    }

    public void AddInterest(int interest) {
	mInterest |= interest;
	mReactor.AddToRegistrationQueue(this);
	mReactor.WakeUp();
    }

    public void RemoveInterest(int interest) {
	mInterest &= ~interest;
	mReactor.AddToRegistrationQueue(this);
	mReactor.WakeUp();
    }

    Reactor GetReactor() { return mReactor; }

    protected InetSocketAddress mAddr;
    protected DispatchHandler mDispatchHandler;
    protected ConnectionMap mConnMap;
    protected Event.Queue mEventQueue;
    protected Reactor mReactor;
    protected AbstractSelectableChannel mChannel;
    protected int mInterest;
    protected boolean mShuttingDown = false;
}
