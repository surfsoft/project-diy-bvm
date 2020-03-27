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

#include "AdafruitMotorShield.h"

AdafruitMotorShield::AdafruitMotorShield() {

    _motorShield = Adafruit_MotorShield();
    _motor1 = _motorShield.getMotor(1);
    _motor2 = _motorShield.getMotor(2);
    _motor3 = _motorShield.getMotor(3);
    _motor4 = _motorShield.getMotor(4);

}

void AdafruitMotorShield::begin() {

    if (_state == STATE_SETUP) {
        _state = STATE_RUNNING;
        _motorShield.begin();
    }

}

void AdafruitMotorShield::run(boolean direction) {

    if (direction == RETRACT) {

#ifdef DEBUG_MOTOR_SHIELD
        Serial.println("<<<< Retracting");
#endif

        _motor1->run(BACKWARD);
        _motor2->run(BACKWARD);
        _motor3->run(BACKWARD);
        _motor4->run(BACKWARD);
    }
    else {

#ifdef DEBUG_MOTOR_SHIELD
        Serial.println("Compressing >>>");
#endif

        _motor1->run(FORWARD);
        _motor2->run(FORWARD);
        _motor3->run(FORWARD);
        _motor4->run(FORWARD);
    }

    _motor1->setSpeed(255);
    _motor2->setSpeed(255);
    _motor3->setSpeed(255);
    _motor4->setSpeed(255);

}

void AdafruitMotorShield::stop() {

    _motor1->setSpeed(0);
    _motor2->setSpeed(0);
    _motor3->setSpeed(0);
    _motor4->setSpeed(0);

#ifdef DEBUG_MOTOR_SHIELD
    Serial.println("<<<<Stopped>>>>");
#endif

}