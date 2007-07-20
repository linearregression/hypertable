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


extern "C" {
#include <stdio.h>
#include <string.h>
}

#include "Common/Error.h"
#include "Common/Usage.h"

#include "Hyperspace/HyperspaceClient.h"

#include "CommandExists.h"

using namespace hypertable;


namespace {
  const char *usage[] = {
    "",
    "usage: hyperspaceClient exists <fname>",
    "",
    "This program tests whether a file named <fname> in HYPERSPACE exists and",
    "displays 'true' if it does and 'false' if it does not.",
    (const char *)0
  };

}

int hypertable::CommandExists(HyperspaceClient *client, vector<const char *> &args) {

  if (args.size() < 1)
    Usage::DumpAndExit(usage);

  int error = client->Exists(args[0]);
  if (error == Error::HYPERTABLEFS_FILE_NOT_FOUND)
    cout << "false" << endl;
  else if (error == Error::OK)
    cout << "true" << endl;
  else
    return error;

  return Error::OK;
}

