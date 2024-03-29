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

#include <cstring>
#include <iostream>
#include <vector>

extern "C" {
#include <readline/readline.h>
#include <readline/history.h>
}

#include <boost/algorithm/string.hpp>

#include "Common/Error.h"
#include "Common/InteractiveCommand.h"
#include "Common/Properties.h"
#include "Common/System.h"
#include "Common/Usage.h"
#include "AsyncComm/Comm.h"

#include "CommandCopyFromLocal.h"
#include "CommandCopyToLocal.h"
#include "CommandLength.h"
#include "CommandMkdirs.h"
#include "CommandRemove.h"
#include "CommandRmdir.h"
#include "CommandShutdown.h"

using namespace Hypertable;
using namespace std;

namespace {

  char *line_read = 0;

  char *rl_gets () {

    if (line_read) {
      free (line_read);
      line_read = (char *)NULL;
    }

    /* Get a line from the user. */
    line_read = readline ("dfsclient> ");

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
      add_history (line_read);

    return line_read;
  }

  const char *usage[] = {
    "usage: dfsclient [OPTIONS]",
    "",
    "OPTIONS:",
    "  --config=<file>  Read configuration from <file>.  The default config file is",
    "                   \"conf/hypertable.cfg\" relative to the toplevel install directory",
    "  --eval <cmds>    Evaluates the commands in the string <cmds>.  Several commands can",
    "                   be supplied in <cmds> by separating them with semicolons",
    "  --help           Display this help text and exit",
    ""
    "This is a command line interface to the DFS broker.",
    (const char *)0
  };
  
}


/**
 *
 */
int main(int argc, char **argv) {
  const char *line;
  char *eval = 0;
  size_t i;
  string configFile = "";
  vector<InteractiveCommand *>  commands;
  Comm *comm;
  ConnectionManagerPtr connManagerPtr;
  DfsBroker::Client *client;
  PropertiesPtr props_ptr;

  System::initialize(argv[0]);
  ReactorFactory::initialize((uint16_t)System::get_processor_count());

  for (int i=1; i<argc; i++) {
    if (!strncmp(argv[i], "--config=", 9))
      configFile = &argv[i][9];
    else if (!strcmp(argv[i], "--eval")) {
      i++;
      if (i == argc)
	Usage::dump_and_exit(usage);
      eval = argv[i];
    }
    else
      Usage::dump_and_exit(usage);
  }

  if (configFile == "")
    configFile = System::installDir + "/conf/hypertable.cfg";

  props_ptr = new Properties( configFile );

  comm = new Comm();
  connManagerPtr = new ConnectionManager(comm);

  client = new DfsBroker::Client(connManagerPtr, props_ptr);

  if (!client->wait_for_connection(15)) {
    LOG_ERROR("Error:  Timed out waiting for DFS broker.");
    exit(1);
  }

  commands.push_back( new CommandCopyFromLocal(client) );
  commands.push_back( new CommandCopyToLocal(client) );
  commands.push_back( new CommandLength(client) );
  commands.push_back( new CommandMkdirs(client) );
  commands.push_back( new CommandRemove(client) );
  commands.push_back( new CommandRmdir(client) );
  commands.push_back( new CommandShutdown(client) );

  /**
   * Non-interactive mode
   */
  if (eval != 0) {
    const char *str;
    std::string commandStr;
    str = strtok(eval, ";");
    while (str) {
      commandStr = str;
      boost::trim(commandStr);
      for (i=0; i<commands.size(); i++) {
	if (commands[i]->matches(commandStr.c_str())) {
	  commands[i]->parse_command_line(commandStr.c_str());
	  if (commands[i]->run() != Error::OK)
	    return 1;
	  break;
	}
      }
      if (i == commands.size()) {
	LOG_VA_ERROR("Unrecognized command : %s", commandStr.c_str());
	return 1;
      }
      str = strtok(0, ";");      
    }
    return 0;
  }

  cout << "Welcome to dsftool, a command-line interface to the DFS broker." << endl;
  cout << "Type 'help' for a description of commands." << endl;
  cout << endl << flush;

  using_history();
  while ((line = rl_gets()) != 0) {

    if (*line == 0)
      continue;

    for (i=0; i<commands.size(); i++) {
      if (commands[i]->matches(line)) {
	commands[i]->parse_command_line(line);
	commands[i]->run();
	break;
      }
    }

    if (i == commands.size()) {
      if (!strcmp(line, "quit") || !strcmp(line, "exit"))
	exit(0);
      else if (!strcmp(line, "help")) {
	cout << endl;
	for (i=0; i<commands.size(); i++) {
	  Usage::dump(commands[i]->usage());
	  cout << endl;
	}
      }
      else
	cout << "Unrecognized command." << endl;
    }

  }

  return 0;
}
