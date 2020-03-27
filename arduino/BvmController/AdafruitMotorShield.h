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

#ifndef AdafruitMotorShield_h

#define AdafruitMotorShield_h

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Uncomment for basic diagnostics
//#define DEBUG_MOTOR_SHIELD

class AdafruitMotorShield {

    public:

        #define EXTEND   true
        #define RETRACT  false

        AdafruitMotorShield();

        void begin();

        void run(boolean direction);

        void stop();

    private:

        #define STATE_SETUP    0
        #define STATE_RUNNING  1

        uint8_t                _state;
        Adafruit_MotorShield   _motorShield;
        Adafruit_DCMotor*      _motor1;
        Adafruit_DCMotor*      _motor2;
        Adafruit_DCMotor*      _motor3;
        Adafruit_DCMotor*      _motor4;

};

#endif
