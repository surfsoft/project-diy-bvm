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

#include "OneShotExecutionHandler.h"

OneShotExecutionHandler::OneShotExecutionHandler(char* name) {

    _name = name;
    _state = STATE_SETUP;

}

/****
 * Public methods
 ****/

void OneShotExecutionHandler::begin(unsigned long executionInterval) {

    if (_state == STATE_SETUP) {
        _state = STATE_RUNNING;
        _nextExecutionTime = millis() + executionInterval;
        _executed = false;
    }

}

boolean OneShotExecutionHandler::isDue() {

    boolean executionDue = false;
    if (_state == STATE_RUNNING) {
        if (!_executed && _nextExecutionTime < millis()) {
            _executed = true;
            executionDue = true;
            _state = STATE_SETUP;
        }
    }

    return executionDue;

}

void OneShotExecutionHandler::cancel() {
    _executed = true;
    _state = STATE_SETUP;
}