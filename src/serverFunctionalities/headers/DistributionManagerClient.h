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

#ifndef Distribution_Manager_CLIENT_H
#define Distribution_Manager_CLIENT_H

#include "ServerFunctionality.h"
#include "PDBLogger.h"
#include "PDBServer.h"


#include "NodeInfo.h"
#include "Ack.h"
#include "QueryPermit.h"
#include "QueryPermitResponse.h"
#include "QueryDone.h"
#include "PlaceOfQueryPlanner.h"

namespace pdb {

class DistributionManagerClient;
typedef shared_ptr<DistributionManagerClient> DistributionManagerClientPtr;

class DistributionManagerClient : public ServerFunctionality {

public:


	DistributionManagerClient (string &masterHostNameIn, int masterNodePortIn);

	~DistributionManagerClient ();

	// function to register event handlers associated with this server functionality
	virtual void registerHandlers (PDBServer &forMe) override;


	// TODO: Not sure if we need a shutDown method for the distribution manager.

//	// shuts down the server that we are connected to... returns true on success
//	bool shutDownServer (std :: string &errMsg);

	// send the HeartBeat info to the server.
	void sendHeartBeat(string &masterHostName, int masterNodePort, pdb::Handle<NodeInfo> m_nodeInfo, PDBLoggerPtr logger, bool &wasError, string& errMsg);

	// send a request for running a query
	Handle<QueryPermitResponse> sendQueryPermitt(string &hostName, int masterNodePort, pdb::Handle<QueryPermit> m_queryPermit, PDBLoggerPtr logger, bool &wasError, string& errMsg);

	// informs the server that running of a specific query is done
	Handle<Ack> sendQueryDone(string &hostName, int masterNodePort, Handle<QueryDone> m_queryDone, PDBLoggerPtr logger, bool &wasError, string& errMsg);

	// sends the place of query planner node to the user-client
	Handle<Ack> sendGetPlaceOfQueryPlanner(string &hostName, int masterNodePort, Handle<PlaceOfQueryPlanner> m_PlaceOfQueryPlanner, PDBLoggerPtr logger, bool &wasError, string& errMsg);

private:

	string masterHostName;
	int masterNodePort;
};

}

#endif
