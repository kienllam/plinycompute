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
/* 
 * File:   SequenceID.h
 * Author: Jia
 *
 * Created on September 28, 2015, 10:10 PM
 */

#ifndef SEQUENCEID_H
#define	SEQUENCEID_H

#include <pthread.h>

class SequenceID {
private:
    unsigned int currentID;
    pthread_mutex_t sequenceIDMutex;

public:

    SequenceID() {
        currentID = 0;
        pthread_mutex_init(&sequenceIDMutex, nullptr);
    }

    ~SequenceID() {
        pthread_mutex_destroy(&sequenceIDMutex);
    }

    void initialize(unsigned int currentID) {
        this->currentID = currentID;
    }

    unsigned int getNextSequenceID() {
        pthread_mutex_lock(&sequenceIDMutex);
        unsigned int nextID = currentID;
        currentID++;
        pthread_mutex_unlock(&sequenceIDMutex);
        return nextID;

    }
};



#endif	/* SEQUENCEID_H */

