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

#ifndef L239D_h

#define L239D_h

#include "Arduino.h"

// Uncomment for basic diagnostics
//#define DEBUG_L239D

class L239D {

    public:

        #define EXTEND   true
        #define RETRACT  false

        L239D(int controlPin1, int controlPin2);

        void begin();

        void run(boolean direction);

        void stop();

    private:

        #define STATE_SETUP    0
        #define STATE_RUNNING  1

        uint8_t                _state;
        uint8_t                _controlPin1;
        uint8_t                _controlPin2;

};

#endif
