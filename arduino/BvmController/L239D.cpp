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

#include "L239D.h"

L239D::L239D(int controlPin1, int controlPin2) {

    _controlPin1 = controlPin1;
    _controlPin2 = controlPin2;

}

void L239D::begin() {

    if (_state == STATE_SETUP) {
        _state = STATE_RUNNING;
        pinMode(_controlPin1, OUTPUT);
        pinMode(_controlPin2, OUTPUT);
    }

}

void L239D::run(boolean direction) {

    if (direction == RETRACT) {

#ifdef DEBUG_L239D
        Serial.println("<<<< Retracting");
#endif

        digitalWrite(_controlPin1, HIGH);
        digitalWrite(_controlPin2, LOW);

    }
    else {

#ifdef DEBUG_L239D
        Serial.println("Compressing >>>");
#endif

        digitalWrite(_controlPin1, LOW);
        digitalWrite(_controlPin2, HIGH);

    }


}

void L239D::stop() {

        digitalWrite(_controlPin1, LOW);
        digitalWrite(_controlPin2, LOW);

#ifdef DEBUG_L239D
    Serial.println("<<<<Stopped>>>>");
#endif

}