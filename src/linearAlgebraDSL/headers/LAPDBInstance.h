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
#ifndef LA_RUNING_ENVIRONMENT_H
#define LA_RUNING_ENVIRONMENT_H

#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>
#include <fcntl.h>
#include <set>
#include <map>

#include "PDBDebug.h"
#include "PDBString.h"
#include "Query.h"
#include "Lambda.h"
#include "QueryClient.h"
#include "DistributedStorageManagerClient.h"
#include "DispatcherClient.h"
#include "Set.h"
#include "DataTypes.h"
#include "LADimension.h"


class LAPDBInstance{

private:
	bool printResult;
    bool clusterMode;
    int blockSize;
    std :: string masterIP;
    int port;
    pdb :: PDBLoggerPtr clientLogger;
    pdb :: DistributedStorageManagerClient storageClient;
    pdb :: CatalogClient catalogClient;
    pdb :: DispatcherClient dispatcherClient;
    pdb :: QueryClient queryClient;

    std::string errMsg;

    int dispatchCount = 0;

    std::set<std::string> cachedSet;
    std::map<std::string,pdb::Handle<pdb::Computation>> identifierComputationMap;
    std::map<std::string,LADimension> identifierDimensionMap;

public:

	LAPDBInstance(bool printResultIn, bool clusterModeIn, int blockSizeIn, std::string masterIPIn, int portIn, pdb::PDBLoggerPtr loggerIn):
		printResult(printResultIn),
		clusterMode(clusterModeIn),
		blockSize(blockSizeIn),
		masterIP(masterIPIn),
		port(portIn),
		clientLogger(loggerIn),
		storageClient(port, masterIP, clientLogger),
		catalogClient(port, masterIP, clientLogger),
		dispatcherClient(port, masterIP, clientLogger),
		queryClient(port,masterIP,clientLogger,true)
	{
		//Register libraries;
		catalogClient.registerType("libraries/libLAMaxElementOutputType.so", errMsg);
		catalogClient.registerType("libraries/libLAMaxElementValueType.so", errMsg);
		catalogClient.registerType("libraries/libLAMinElementOutputType.so", errMsg);
		catalogClient.registerType("libraries/libLAMinElementValueType.so", errMsg);
		catalogClient.registerType("libraries/libLAScanMatrixBlockSet.so", errMsg);
		catalogClient.registerType("libraries/libLASillyAddJoin.so", errMsg);
		catalogClient.registerType("libraries/libLASillyRowMaxAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyRowMinAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyColMaxAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyColMinAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillySubstractJoin.so", errMsg);
		catalogClient.registerType("libraries/libLASillyMaxElementAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyMinElementAggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyMultiply1Join.so", errMsg);
		catalogClient.registerType("libraries/libLASillyMultiply2Aggregate.so", errMsg);
		catalogClient.registerType("libraries/libLASillyTransposeMultiply1Join.so", errMsg);
		catalogClient.registerType("libraries/libLASillyTransposeSelection.so", errMsg);
		catalogClient.registerType("libraries/libLAWriteMatrixBlockSet.so", errMsg);
		catalogClient.registerType("libraries/libLAWriteMaxElementSet.so", errMsg);
		catalogClient.registerType("libraries/libLAWriteMinElementSet.so", errMsg);
		catalogClient.registerType("libraries/libMatrixMeta.so", errMsg);
		catalogClient.registerType("libraries/libMatrixData.so", errMsg);
		catalogClient.registerType("libraries/libMatrixBlock.so", errMsg);

		if (!storageClient.createDatabase ("LA_db", errMsg)) {
            std::cout << "Not able to create database: " + errMsg;
            exit (-1);
        } else {
            std::cout << "Created database <LA_db>.\n";
        }
	}


	pdb :: DistributedStorageManagerClient& getStorageClient(){
		return this->storageClient;
	}

    pdb :: DispatcherClient& getDispatchClient(){
    	return this->dispatcherClient;
    }

    pdb :: QueryClient& getQueryClient(){
    	return this->queryClient;
    }

    void increaseDispatchCount(){
    	this->dispatchCount += 1;
    }

    int getDispatchCount(){
    	return dispatchCount;
    }

    int getBlockSize(){
    	return blockSize;
    }

    std :: string & instanceErrMsg(){
    	return errMsg;
    }

    void addToCachedSet(std::string setName){
    	cachedSet.insert(setName);
    }

    void deleteFromCachedSet(std::string setName){
    	cachedSet.erase(setName);
    }

    void addToIdentifierComputationMap(std::string identiferName,pdb::Handle<pdb::Computation> scanSet){
    	if(identifierComputationMap.find(identiferName)!=identifierComputationMap.end()){
    		identifierComputationMap.erase(identiferName);
    	}
    	identifierComputationMap.insert(std::pair<std::string,pdb::Handle<pdb::Computation>>(identiferName,scanSet));
    }

    bool existsScanSet(std::string identiferName){
        return identifierComputationMap.find(identiferName)!=identifierComputationMap.end();
    }

    pdb::Handle<pdb::Computation> findScanSet(std::string identiferName){
    	return identifierComputationMap[identiferName];
    }

    void addToIdentifierDimensionMap(std::string identiferName,LADimension dim){
    	if(identifierDimensionMap.find(identiferName)!=identifierDimensionMap.end()){
    		identifierDimensionMap.erase(identiferName);
    	}
    	identifierDimensionMap.insert(std::pair<std::string,LADimension>(identiferName,dim));
    }

    LADimension findDimension(std::string identiferName){
    	return identifierDimensionMap[identiferName];
    }

    void clearCachedSets(){
        std::cout << "Clear all the set in LA_db." <<std::endl;
        for(auto const &setName : cachedSet){
            if(!storageClient.removeSet("LA_db",setName,errMsg)){
                std::cout << "Not able to remove set: " + errMsg << std::endl;
                exit (-1);
            }
            else{
                std::cout << "Set " << setName << " removed" <<std::endl;
            }
        }
        cachedSet.clear();
    }
};

#endif