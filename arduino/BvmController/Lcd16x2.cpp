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
 *
 *  Display panel is two lines of sixteen characters, formatted as follows:
 *          1234567890123456
 *  Line 1:  SM 99 SS 99 A999
 *  Line 2:  BS 999 HHH:MM:SS
 *
 *  SM: Squeezes per Minute
 *  SS: Squeeze size (percentage of bag size)
 *  A:  Actuator speed in mm/s
 *  BS: Bag size in mm
 *  HH:MM:SS is the run time
 */

#include "Lcd16x2.h"

Lcd16x2::Lcd16x2(LiquidCrystal* lcd) {
    _lcd = lcd;
}

void Lcd16x2::begin() {

    if (_state == STATE_SETUP) {
        _state = STATE_RUNNING;
       _lcd->begin(16, 2);
    }

}

void Lcd16x2::update(long breathsPerMinute, long breathSize, long actuatorSpeed, long bagSize, boolean running, unsigned long runTime) {

    if (_state == STATE_RUNNING) {

        unsigned long runTimeSeconds = runTime / 1000L;
        unsigned long hours = runTimeSeconds / 3600L;
        unsigned long minutes = (runTimeSeconds % 3600) / 60L;
        unsigned long seconds = runTimeSeconds % 60;

        // Fist line
        _lcd->setCursor(0, 0);

        // Squeezes per minute
        _lcd->print("SM ");
        if (breathsPerMinute < 10) {
            _lcd->print(" ");
        }
        _lcd->print(breathsPerMinute);
        _lcd->print(" ");

        // Squeeze size
        _lcd->print("SS ");
        if (breathSize < 10) {
            _lcd->print(" ");
        }
        _lcd->print(breathSize);
        _lcd->print(" ");

        // Actuator speed
        _lcd->print("A");
        if (actuatorSpeed < 100) {
            _lcd->print(" ");
            if (actuatorSpeed < 10) {
                _lcd->print(" ");
            }
        }
        _lcd->print(actuatorSpeed);

        // Second line
        _lcd->setCursor(0, 1);

        // Bag size
        _lcd->print("BS ");
        if (bagSize < 100) {
            _lcd->print(" ");
        }
        _lcd->print(bagSize);
        _lcd->print(" ");

        if (running) {
            // Run time
            if (hours < 100) {
                _lcd->print(" ");
                if (hours < 10) {
                    _lcd->print(" ");
                }
            }
            _lcd->print(hours);
            _lcd->print(":");
            if (minutes < 10) {
                _lcd->print("0");
            }
            _lcd->print(minutes);
            _lcd->print(":");
            if (seconds < 10) {
                _lcd->print("0");
            }
            _lcd->print(seconds);
        }
        else {
            _lcd->print("==RESET==");
        }

    }

}