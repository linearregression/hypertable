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

#include <cstdio>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/thread/exceptions.hpp>

extern "C" {
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/poll.h>
}

#include "AsyncComm/Comm.h"
#include "AsyncComm/CommBuf.h"
#include "AsyncComm/HeaderBuilder.h"
#include "Common/Error.h"
#include "Common/InetAddr.h"
#include "Common/InteractiveCommand.h"
#include "Common/Logger.h"
#include "Common/Properties.h"
#include "Common/System.h"
#include "Common/Usage.h"

#include "Global.h"
#include "Hyperspace/Session.h"

#include "CommandMkdir.h"
#include "CommandDelete.h"
#include "CommandOpen.h"
#include "CommandCreate.h"
#include "CommandClose.h"
#include "CommandAttrSet.h"
#include "CommandAttrGet.h"
#include "CommandAttrDel.h"
#include "CommandExists.h"
#include "CommandReaddir.h"
#include "CommandLock.h"
#include "CommandTryLock.h"
#include "CommandRelease.h"
#include "CommandGetSequencer.h"

using namespace hypertable;
using namespace std;


namespace {

  char *line_read = 0;

  bool gTestMode = false;
  std::string gInputStr;

  char *rl_gets () {

    if (gTestMode) {
      if (!getline(cin, gInputStr))
	return 0;
      boost::trim(gInputStr);
      if (gInputStr.find("echo", 0) != 0 && gInputStr.find("quit", 0) != 0)
	cout << gInputStr << endl;
      return (char *)gInputStr.c_str();
    }
    else {
      if (line_read) {
	free (line_read);
	line_read = (char *)NULL;
      }

      /* Get a line from the user. */
      line_read = readline("hyperspace> ");

      /* If the line has any text in it, save it on the history. */
      if (line_read && *line_read)
	add_history (line_read);

      return line_read;
    }
  }

  const char *usage[] = {
    "usage: hyperspace [OPTIONS]",
    "",
    "OPTIONS:",
    "  --config=<file>  Read configuration from <file>.  The default config file is",
    "                   \"conf/hypertable.cfg\" relative to the toplevel install",
    "                   directory",
    "  --debug          Turn on debugging output",
    "  --eval <cmds>    Evaluates the commands in the string <cmds>.  Several",
    "                   commands can be supplied in <cmds> by separating them with",
    "                   semicolons",
    "  --no-prompt      Don't display a command prompt",
    "  --notification-address=[<host>:]<port>  Send notification datagram to this",
    "                   address after each command.",
    "  --help           Display this help text and exit",
    "  --test-mode      Suppress file line number information from error messages to",
    "                   simplify diff'ing test output.",
    "",
    "This is a command interpreter for Hyperspace, a global namespace and lock service",
    "for loosely-coupled distributed systems.",
    (const char *)0
  };

  const char *helpTrailer[] = {
    "echo <str>",
    "  Display <str> to stdout.",
    "",
    "help",
    "  Display this help text.",
    "",
    "quit",
    "  Exit the program.",
    "",
    (const char *)0
  };
}

/**
 *
 */
class Notifier {

public:

  Notifier(const char *addressStr) {
    int error;
    mComm = new Comm();
    if (!InetAddr::Initialize(&mAddr, addressStr)) {
      exit(1);
    }
    InetAddr::Initialize(&mSendAddr, INADDR_ANY, 0);
    if ((error = mComm->CreateDatagramReceiveSocket(&mSendAddr, 0)) != Error::OK) {
      std::string str;
      LOG_VA_ERROR("Problem creating UDP receive socket %s - %s", InetAddr::StringFormat(str, mSendAddr), Error::GetText(error));
      exit(1);
    }
  }

  Notifier() : mComm(0) {
    return;
  }

  void notify() {
    if (mComm) {
      int error;
      CommBufPtr cbufPtr( new CommBuf(mBuilder, 2) );
      cbufPtr->AppendShort(0);
      if ((error = mComm->SendDatagram(mAddr, mSendAddr, cbufPtr)) != Error::OK) {
	LOG_VA_ERROR("Problem sending datagram - %s", Error::GetText(error));
	exit(1);
      }
    }
  }

private:
  Comm *mComm;
  struct sockaddr_in mAddr;
  struct sockaddr_in mSendAddr;
  HeaderBuilder mBuilder;
};


/**
 * 
 */
class SessionHandler : public SessionCallback {
public:
  virtual void Jeopardy() { cout << "SESSION CALLBACK: Jeopardy" << endl << flush; }
  virtual void Safe() { cout << "SESSION CALLBACK: Safe" << endl << flush; }
  virtual void Expired() { cout << "SESSION CALLBACK: Expired" << endl << flush; }
};



/**
 *  main
 */
int main(int argc, char **argv, char **envp) {
  const char *line;
  char *eval = 0;
  size_t i;
  string configFile = "";
  vector<InteractiveCommand *>  commands;
  Comm *comm;
  ConnectionManager *connManager;
  PropertiesPtr propsPtr;
  Hyperspace::Session *session;
  SessionHandler sessionHandler;
  bool verbose = false;
  int error;
  Notifier *notifier = 0;

  System::Initialize(argv[0]);
  ReactorFactory::Initialize((uint16_t)System::GetProcessorCount());

  configFile = System::installDir + "/conf/hypertable.cfg";

  for (int i=1; i<argc; i++) {
    if (!strncmp(argv[i], "--config=", 9))
      configFile = &argv[i][9];
    else if (!strcmp(argv[i], "--debug"))
      verbose = true;
    else if (!strcmp(argv[i], "--eval")) {
      i++;
      if (i == argc)
	Usage::DumpAndExit(usage);
      eval = argv[i];
    }
    else if (!strcmp(argv[i], "--test-mode")) {
      Logger::SetTestMode("hyperspace");
      gTestMode = true;
    }
    else if (!strncmp(argv[i], "--notification-address=", 23))
      notifier = new Notifier(&argv[i][23]);
    else
      Usage::DumpAndExit(usage);
  }

  if (notifier == 0)
    notifier = new Notifier();

  propsPtr.reset( new Properties( configFile ) );

  if (verbose)
    propsPtr->setProperty("verbose", "true");

  comm = new Comm();

  session = new Session(comm, propsPtr, &sessionHandler);

  if (!session->WaitForConnection(30)) {
    cerr << "Unable to establish session with Hyerspace, exiting..." << endl;
    exit(1);
  }

  commands.push_back( new CommandMkdir(session) );
  commands.push_back( new CommandDelete(session) );
  commands.push_back( new CommandOpen(session) );
  commands.push_back( new CommandCreate(session) );
  commands.push_back( new CommandClose(session) );
  commands.push_back( new CommandAttrSet(session) );
  commands.push_back( new CommandAttrGet(session) );
  commands.push_back( new CommandAttrDel(session) );
  commands.push_back( new CommandExists(session) );
  commands.push_back( new CommandReaddir(session) );
  commands.push_back( new CommandLock(session) );
  commands.push_back( new CommandTryLock(session) );
  commands.push_back( new CommandRelease(session) );
  commands.push_back( new CommandGetSequencer(session) );

  /**
   * Non-interactive mode
   */
  if (eval != 0) {
    const char *str;
    std::string commandStr;
    str = strtok(eval, ";");
    while (str) {
      Global::exitStatus = 0;
      commandStr = str;
      boost::trim(commandStr);
      for (i=0; i<commands.size(); i++) {
	if (commands[i]->Matches(commandStr.c_str())) {
	  commands[i]->ParseCommandLine(commandStr.c_str());
	  if ((error = commands[i]->run()) != Error::OK) {
	    cerr << Error::GetText(error) << endl;
	    return 1;
	  }
	  break;
	}
      }
      if (i == commands.size()) {
	LOG_VA_ERROR("Unrecognized command : %s", commandStr.c_str());
	return 1;
      }
      str = strtok(0, ";");      
    }
    return Global::exitStatus;
  }

  cout << "Welcome to the Hyperspace command interpreter.  Hyperspace" << endl;
  cout << "is a global namespace and lock service for loosely-coupled" << endl;
  cout << "distributed systems.  Type 'help' for a description of commands." << endl;
  cout << endl << flush;

  using_history();
  while ((line = rl_gets()) != 0) {

    Global::exitStatus = 0;

    if (*line == 0)
      continue;

    for (i=0; i<commands.size(); i++) {
      if (commands[i]->Matches(line)) {
	commands[i]->ParseCommandLine(line);
	if ((error = commands[i]->run()) != Error::OK && error != -1)
	  cout << Error::GetText(error) << endl;
	notifier->notify();
	break;
      }
    }

    if (i == commands.size()) {
      if (!strcmp(line, "quit") || !strcmp(line, "exit")) {
	notifier->notify();
	exit(0);
      }
      else if (!strncmp(line, "echo", 4)) {
	std::string echoStr = std::string(line);
	echoStr = echoStr.substr(4);
	boost::trim_if(echoStr, boost::is_any_of("\" \t"));
	cout << echoStr << endl;
	notifier->notify();
      }
      else if (!strcmp(line, "pwd")) {
	cout << Global::cwd << endl;
	notifier->notify();
      }
      else if (!strcmp(line, "help")) {
	cout << endl;
	for (i=0; i<commands.size(); i++) {
	  Usage::Dump(commands[i]->Usage());
	  cout << endl;
	}
	Usage::Dump(helpTrailer);
	notifier->notify();
      }
      else {
	cout << "Unrecognized command." << endl;
	notifier->notify();
      }
    }
  }

  delete session;
  delete comm;
  return Global::exitStatus;
}

