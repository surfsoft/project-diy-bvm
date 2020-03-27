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

#ifndef Lcd16x2_h

#define Lcd16x2_h

#include "Arduino.h"
#include <LiquidCrystal.h>

class Lcd16x2 {

    public:

        Lcd16x2(LiquidCrystal *lcd);

        void begin();

        void update(long breathsPerMinute, long breathSize, long actuatorSpeed, long bagSize, boolean running, unsigned long runTime);

    private:

        #define STATE_SETUP    0
        #define STATE_RUNNING  1

        uint8_t                _state;
        LiquidCrystal*         _lcd;

};

#endif
