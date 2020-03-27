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

#ifndef AsynchronousExecutionHandler_h

#define AsynchronousExecutionHandler_h

#include "Arduino.h"

class AsynchronousExecutionHandler {

    public:

        AsynchronousExecutionHandler(char* name);


        // Starts execution
        void begin(unsigned long executionInterval);

        void begin(unsigned long executionInterval, boolean paused);

        // Decides if the functionToExecute should be invoked or not
        boolean isDue();

        // Pauses execution of functionToExecute until resume() is invoked
        void pause();

        // Resumes execution of functionToExecute until pause() is invoked
        void resume();

        // Returns true if the handler is paused
        boolean isPaused();

    private:

        #define STATE_SETUP               0
        #define STATE_RUNNING             1

        uint8_t                    _state;
        char*                      _name;
        unsigned long              _executionInterval;
        unsigned long              _nextExecutionTime;

        boolean                    _paused;

};

#endif
