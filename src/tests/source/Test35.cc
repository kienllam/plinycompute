/*****************************************************************************
 *                                                                           *
 *  Copyright 2018 Rice University                                           *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *      http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef TEST_35_CC
#define TEST_35_CC

#include "PDBServer.h"
#include "ResourceManagerServer.h"
#include "QuerySchedulerServer.h"
#include "CatalogServer.h"
#include "CatalogClient.h"
#include "StorageServer.h"
#include "Configuration.h"
#include "PangeaStorageServer.h"
#include "SharedMem.h"

int main () {

       std :: cout << "Starting up a resource manager/query scheduler server!!\n";
       pdb :: PDBLoggerPtr myLogger = make_shared <pdb :: PDBLogger> ("frontendLogFile.log");
       pdb :: PDBServer frontEnd (8108, 10, myLogger);
       frontEnd.addFunctionality <pdb :: CatalogServer> ("CatalogDir", true);
       frontEnd.addFunctionality <pdb :: CatalogClient> (8108, "localhost", myLogger);

       ConfigurationPtr conf = make_shared < Configuration > ();
       pdb :: PDBLoggerPtr logger = make_shared < pdb :: PDBLogger> (conf->getLogFile());
       SharedMemPtr shm = make_shared< SharedMem > (conf->getShmSize(), logger);
       frontEnd.addFunctionality<pdb :: PangeaStorageServer> (shm, frontEnd.getWorkerQueue(), logger, conf);
       frontEnd.getFunctionality<pdb :: PangeaStorageServer>().startFlushConsumerThreads();

       frontEnd.addFunctionality <pdb :: ResourceManagerServer> ("conf/serverlist", 8101);
       frontEnd.addFunctionality <pdb :: QuerySchedulerServer> ("localhost", 8108, myLogger);
       frontEnd.startServer (nullptr);
}

#endif

