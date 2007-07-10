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



package org.hypertable.Hypertable;

import java.io.File;
import java.io.IOException;
import java.lang.StringBuilder;
import java.util.LinkedList;
import java.util.logging.Logger;

import org.hypertable.Common.Error;
import org.hypertable.Common.FileUtils;

import org.hypertable.AsyncComm.CallbackHandlerSynchronizer;
import org.hypertable.AsyncComm.Event;

class CommandOpenTable extends Command {

    static final Logger log = Logger.getLogger("org.hypertable.Hypertable");

    public CommandOpenTable(String argsStr) throws IOException {
	super(argsStr);
    }

    public void run() {
	int msgId;
	CallbackHandlerSynchronizer syncHandler = new CallbackHandlerSynchronizer(30);

	if (mArgs.size() != 1)
	    ReportError("error: wrong number of arguments.");
	else {
	    String tableName = mArgs.elementAt(0).sval;
	    if (tableName == null) {
		ReportError("error: Bad table name");
	    }
	    else {
		try {

		    if (Global.tableMap.containsKey(tableName)) {
			log.severe("Table '" + tableName + "' is already open");
			return;
		    }
		    else {
			StringBuilder sb = new StringBuilder();

			int error = Global.master.GetSchema(tableName, sb);
			if (error != Error.OK) {
			    log.severe("Error getting schema for table '" + tableName + "' - " + Error.GetText(error));
			    return;
			}

			Schema schema = new Schema(sb.toString());
			Table table = new Table(schema);
			Global.tableMap.put(tableName, table);
		    }
		}
		catch (Exception e) {
		    e.printStackTrace();
		    System.exit(1);
		}
	    }
	}
    }
}
