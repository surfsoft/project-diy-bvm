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

#include "AsynchronousExecutionHandler.h"

AsynchronousExecutionHandler::AsynchronousExecutionHandler(char* name) {

    _name = name;
    _state = STATE_SETUP;
    _paused = false;

}

/****
 * Public methods
 ****/

void AsynchronousExecutionHandler::begin(unsigned long executionInterval) {
    begin(executionInterval, false);
}

void AsynchronousExecutionHandler::begin(unsigned long executionInterval, boolean paused) {

    if (_state == STATE_SETUP) {
        _state = STATE_RUNNING;
        _executionInterval = executionInterval;
        _nextExecutionTime = millis() + _executionInterval;
        _paused = true;
        if (!paused) {
            resume();
        }
    }

}

boolean AsynchronousExecutionHandler::isDue() {

    boolean executionDue = false;
    if (_state == STATE_RUNNING) {
        if (!_paused && _nextExecutionTime < millis()) {
            _nextExecutionTime = millis() + _executionInterval;
            executionDue = true;
        }
    }

    return executionDue;

}

void AsynchronousExecutionHandler::pause() {

    if (_state == STATE_RUNNING && !_paused) {
        _paused = true;
    }

}

void AsynchronousExecutionHandler::resume() {

    if (_state == STATE_RUNNING && _paused) {
        _paused = false;
    }

}

boolean AsynchronousExecutionHandler::isPaused() {
    return _paused;
}