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

#ifndef HYPERTABLE_INTERACTIVECOMMAND_H
#define HYPERTABLE_INTERACTIVECOMMAND_H

#include <cstring>

#include <utility>
#include <vector>

namespace hypertable {

  class InteractiveCommand {

  public:

    void ParseCommandLine(const char *line);
    bool Matches(const char *line) { return !strncmp(line, CommandText(), strlen(CommandText())); }

    virtual const char *CommandText() = 0;
    virtual const char **Usage() = 0;
    virtual int run() = 0;

    void ClearArgs() { mArgs.clear(); }
    void PushArg(std::string key, std::string value) { 
      mArgs.push_back( std::pair<std::string, std::string>(key, value) );
    }

  protected:
    std::vector< std::pair<std::string, std::string> >  mArgs;

    bool ParseStringLiteral(const char *str, std::string &text, const char **endptr);
  };

}

#endif // HYPERTABLE_INTERACTIVECOMMAND_H