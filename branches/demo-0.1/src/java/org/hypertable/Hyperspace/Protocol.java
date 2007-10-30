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

package org.hypertable.Hyperspace;

public class Protocol extends org.hypertable.AsyncComm.Protocol {

    public static final short COMMAND_CREATE  = 0;
    public static final short COMMAND_DELETE  = 1;
    public static final short COMMAND_MKDIRS  = 2;
    public static final short COMMAND_ATTRSET = 3;
    public static final short COMMAND_ATTRGET = 4;
    public static final short COMMAND_ATTRDEL = 5;
    public static final short COMMAND_EXISTS  = 6;
    public static final short COMMAND_MAX     = 7;

    public String CommandText(short command) {
	if (command < 0 || command >= COMMAND_MAX)
	    return "UNKNOWN (" + command + ")";
	return msCommandStrings[command];
    }

    public static String msCommandStrings[] = {
	"create",
	"delete",
	"mkdirs",
	"attrset",
	"attrget",
	"attrdel",
	"exists"
    };
}