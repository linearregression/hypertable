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

#include <cassert>

#include "Common/Error.h"
#include "Common/InetAddr.h"
#include "Common/Logger.h"
#include "Common/Properties.h"
#include "Common/System.h"

#include "Manager.h"
#include "Table.h"



/**
 *
 */
Manager::Manager(std::string configFile) {
  uint16_t masterPort;
  const char *masterHost;
  struct sockaddr_in addr;
  PropertiesPtr propsPtr( new Properties(configFile) );

  mInstPtr.reset( new InstanceData() );

  mInstPtr->comm = new Comm();
  mInstPtr->connectionManager = new ConnectionManager(mInstPtr->comm);

  /**
   *  Establish connection to Master
   */
  {
    if ((masterPort = (uint16_t)propsPtr->getPropertyInt("Hypertable.Master.port", 0)) == 0) {
      LOG_ERROR("Hypertable.Master.port property not specified.");
      exit(1);
    }

    if ((masterHost = propsPtr->getProperty("Hypertable.Master.host", (const char *)0)) == 0) {
      LOG_ERROR("Hypertable.Master.host property not specified.");
      exit(1);
    }

    if (!InetAddr::Initialize(&addr, masterHost, masterPort))
      exit(1);
  }

  mInstPtr->connectionManager->Add(addr, 30, "Master");

  // connect to master
  if (!mInstPtr->connectionManager->WaitForConnection(addr, 30)) {
    cerr << "Unable to connect to Master, exiting..." << endl;
    exit(1);
  }

  /**
   *  Create Master client object
   */
  mInstPtr->masterPtr.reset( new MasterClient(mInstPtr->comm, addr) );

}


/**
 * 
 */
int Manager::CreateTable(std::string name, std::string schema) {
  return mInstPtr->masterPtr->CreateTable(name.c_str(), schema.c_str());
}


/**
 *
 */
int Manager::OpenTable(std::string name, TablePtr &tablePtr) {
  Table *table;
  try {
    table = new Table(mInstPtr, name);
  }
  catch (Exception &e) {
    LOG_VA_ERROR("Problem opening table '%s' - %s", name.c_str(), e.what());
    return e.code();
  }
  tablePtr = table;
  return Error::OK;
}



/**
 * 
 */
int Manager::GetSchema(std::string tableName, std::string &schema) {
  return mInstPtr->masterPtr->GetSchema(tableName.c_str(), schema);
}

