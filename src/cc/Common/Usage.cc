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

#include <iostream>

extern "C" {
#include <stdlib.h>
}

#include "Usage.h"

using namespace Hypertable;
using namespace std;


void Usage::dump(const char **usage) {
  for (int i=0; usage[i] != 0; i++)
    cout << usage[i] << endl;
}


void Usage::dump_and_exit(const char **usage, int rcode) {
  cout << endl;
  dump(usage);
  cout << endl;
  exit(1);
}
