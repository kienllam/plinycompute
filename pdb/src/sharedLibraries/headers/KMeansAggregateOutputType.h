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
#ifndef K_MEANS_AGGREGATE_OUTPUT_H
#define K_MEANS_AGGREGATE_OUTPUT_H

#include "Object.h"
#include "Handle.h"
#include "KMeansCentroid.h"

using namespace pdb;

class KMeansAggregateOutputType : public Object {

private:
    int key = 1;
    KMeansCentroid value;

public:
    ENABLE_DEEP_COPY

    KMeansAggregateOutputType() {}

    int& getKey() {
        return key;
    }

    KMeansCentroid& getValue() {
        return value;
    }

    void print() {
        std::cout << "KMeansAggregateOutputType: index: " << key << " value: "
                  << "." << std::endl;
    }

    ~KMeansAggregateOutputType() {}
};

#endif
