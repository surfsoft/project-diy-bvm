/**
 *  Copyright 2020 Surfsoft Consulting Ltd
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef OneShotExecutionHandler_h

#define OneShotExecutionHandler_h

#include "Arduino.h"

class OneShotExecutionHandler {

    public:

        OneShotExecutionHandler(char* name);

        void begin(unsigned long executionInterval);

        void cancel();

        // Returns true when action should be taken
        boolean isDue();

    private:

        #define STATE_SETUP               0
        #define STATE_RUNNING             1

        uint8_t                    _state;
        char*                      _name;
        unsigned long              _nextExecutionTime;
        boolean                    _executed;

};

#endif
