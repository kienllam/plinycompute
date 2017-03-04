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
#pragma once

//PRELOAD %BuiltinTopKInput <Nothing>%
#include "Object.h"
#include "InterfaceFunctions.h"

namespace pdb {

/* A class to wrap TopK input for PDB */
template <class TypeContained>
class BuiltinTopKInput : public Object {

public:

    ENABLE_DEEP_COPY

    BuiltinTopKInput() {}
    ~BuiltinTopKInput() {}

    BuiltinTopKInput(double score, Handle<TypeContained> object) {
        this->score = score;
        this->object = object;
    }

    double getScore() {
        return score;
    }

    Handle<TypeContained> getObject() {
        return object;
    }


private:

    double score;
    Handle<TypeContained> object;


};

}
