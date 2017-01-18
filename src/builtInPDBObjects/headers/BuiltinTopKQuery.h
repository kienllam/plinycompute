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

#ifndef BUILTIN_TOPK_QUERY_H
#define BUILTIN_TOPK_QUERY_H

// PRELOAD %BuiltinTopKQuery%
#ifndef MAX_THREADS
   #define MAX_THREADS 8
#endif

#ifndef K
   #define K 100
#endif

//#define ACCURATE_TOPK

#include "BuiltinTopKInput.h"
#include "BuiltinTopKResult.h"
#include "Selection.h"
#include "PDBMap.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>



namespace pdb {
class BuiltinTopKQuery : public Selection <BuiltinTopKResult, Object> {

public:

       ENABLE_DEEP_COPY

       BuiltinTopKQuery () { 
         this->k = K; 
       }

       BuiltinTopKQuery (int k) {
         this->k = k;
      }


       ~BuiltinTopKQuery() {
           counters = nullptr;
           partialResults = nullptr;
       }


       virtual double getScore(Handle<Object> object) {
           return 3.14159265359;
       };


       void initialize () {
           counters = makeObject<Map<pthread_t, unsigned long>>(MAX_THREADS);
           partialResults = makeObject<Map<pthread_t, Handle<BuiltinTopKResult>>>(MAX_THREADS);

       }



       SimpleLambda <bool> getSelection (Handle <Object> &checkMe) override {
		return makeLambda (checkMe, [&] () {
                    return false; //getSelection will not be applied in pipeline, so simply return false
		});
       }


       SimpleLambda <bool> getProjectionSelection (Handle<BuiltinTopKResult> &checkMe) override {
                return makeLambda (checkMe, [&] () {
                    #ifdef ACCURATE_TOPK
                       return false; //if we do accurate topK
                    #else
                        if (checkMe == nullptr) {
                            return false;
                        } else {
                            return true;
                        }
                    #endif
                });
       }



       SimpleLambda <Handle <BuiltinTopKResult>> getProjection (Handle<Object> &checkMe) override {
                 
		return makeLambda (checkMe, [&] {
                        pthread_t threadId = pthread_self();
                        if (counters->count(threadId) == 0) {
                            UseTemporaryAllocationBlock tempBlock{8*1024*1024};
                            std::cout << "to allocate slot for thread:"<<(unsigned long)(threadId)<<std::endl;
                            (*counters)[threadId] = 0;
                            Handle<BuiltinTopKResult> partialResult = makeObject<BuiltinTopKResult> ();
                            partialResult->initialize();
                            (*partialResults)[threadId] = partialResult;
                        }  
                        #ifndef ACCURATE_TOPK
                        if ((*counters)[threadId] == 10000) {
                            //(*partialResults)[threadId]->reset(); //to clear partial results for last emission
                            (*partialResults)[threadId]->initialize();
                            (*counters)[threadId] = 0; // to clear counter for last emission
                        }
                        #endif
                        double score = getScore(checkMe);
                        (*partialResults)[threadId]->updateTopK(score, checkMe);
                        (*counters)[threadId] ++;

                        Handle<BuiltinTopKResult> ret = nullptr;
                        #ifdef ACCURATE_TOPK
                        return ret;
                        #else
                        if ((*counters)[threadId] == 10000) {
                            ret = makeObject<BuiltinTopKResult>();
                            (*ret) = *(*partialResults)[threadId];
                            Handle<Vector<Handle<BuiltinTopKInput>>> elements = ret->getTopK();
                            int i;
                            for (i = 0; i < elements->size(); i ++) {
                                std::cout << "score=" << (*elements)[i]->getScore() << std::endl;
                            }
                            std :: cout << "emit partialresult with "<< ret->getTopK()->size() << " elements" << std :: endl;
                             
                        }
                        return ret;
                        #endif
		});
	}

        Handle<Vector<Handle<BuiltinTopKResult>>> getAggregatedResults () override  {
            UseTemporaryAllocationBlock tempBlock {64*1024*1024};
            Handle<Vector<Handle<BuiltinTopKResult>>> vectorOfResults = makeObject<Vector<Handle<BuiltinTopKResult>>> ();
            int i = 0;
            for (auto iter = partialResults->begin();  iter != partialResults->end(); ++iter) {
                (*vectorOfResults)[i]= deepCopyToCurrentAllocationBlock<BuiltinTopKResult>( (*iter).value );
            }
            PDB_COUT << "there are " << i << " partial results" << std :: endl;
            return vectorOfResults;
        }


private:

        //counters to record the number of points processed
        Handle<Map<pthread_t, unsigned long>> counters;


        //current aggregated TopK values
        //each thread has a BuiltinTopKResult instance
        Handle<Map<pthread_t, Handle<BuiltinTopKResult>>> partialResults;

        //number of top elements
        int k;
};

}
#endif
