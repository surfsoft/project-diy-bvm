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
 */

#include "AsynchronousExecutionHandler.h"
#include "OneShotExecutionHandler.h"
#include "Lcd16x2.h"

// Causes basic debug messages to be written to the console (may affect timings)
#define DEBUG

/**
 * ===============
 * System defaults
 * ===============
 *
 * Do not change these values if you do not understand the relationships between them.
 */

// The 'width' of the bag as presented to the actuator in mm
#define DEFAULT_BAG_SIZE 150L

// The speed of the actuator in mm per second
#define DEFAULT_ACTUATOR_SPEED 100L

// The maximum number of breaths per minute, limited by the speed of the actuator and size of the bag
#define MAX_BREATHS_PER_MINUTE 20L

// The minimum number of breaths per minute
#define MIN_BREATHS_PER_MINUTE 10L

// The maximum size of a breath (percent of bag size)
#define MAX_BREATH_SIZE 95L

// The minimum size of a breath (percent of bag size)
#define MIN_BREATH_SIZE 20L


/**
 * =======================
 * Arduino pin assignments
 * =======================
 */

// Analogue pins
#define BREATHS_PER_MINUTE             0
#define BREATH_SIZE                    1

// Digital pins: 0-13, 16-19 (14 and 15 are A0 and A1)

// Spare pins
#define BARE_L239D_CONTROL_PIN_1       0
#define BARE_L239D_CONTROL_PIN_2       1

// LCD pins
#define LCD_RS                        12
#define LCD_EN                        11
#define LCD_D4                         5
#define LCD_D5                         4
#define LCD_D6                         3
#define LCD_D7                         2

// Adjustment pins
#define BAG_SIZE_ADJUST_ENABLE         6
#define BAG_SIZE_SIGN                  7
#define BAG_SIZE_ADJUST_BIT_0          8
#define BAG_SIZE_ADJUST_BIT_1          9
#define BAG_SIZE_ADJUST_BIT_2         10

#define ACTUATOR_SPEED_ADJUST_ENABLE  13
#define ACTUATOR_SPEED_SIGN           16
#define ACTUATOR_SPEED_ADJUST_BIT_0   17
#define ACTUATOR_SPEED_ADJUST_BIT_1   18
#define ACTUATOR_SPEED_ADJUST_BIT_2   19


/**
 * ================================================================
 * Functionality to read adjustments to bag size and actuator speed
 * ================================================================
 */


long readBit(long pinNo, long value) {
    return digitalRead(pinNo) == 0 ? 0 : value;
}

long calculateBagSize() {

    pinMode(BAG_SIZE_ADJUST_ENABLE, INPUT_PULLUP);
    pinMode(BAG_SIZE_SIGN, INPUT_PULLUP);
    pinMode(BAG_SIZE_ADJUST_BIT_0, INPUT_PULLUP);
    pinMode(BAG_SIZE_ADJUST_BIT_1, INPUT_PULLUP);
    pinMode(BAG_SIZE_ADJUST_BIT_2, INPUT_PULLUP);

    long sign = 1;
    long delta = 0;

    if (digitalRead(BAG_SIZE_ADJUST_ENABLE) == 0) {
        sign = digitalRead(BAG_SIZE_SIGN) == 0 ? -1 : 1;
        delta = readBit(BAG_SIZE_ADJUST_BIT_0, 1)
              + readBit(BAG_SIZE_ADJUST_BIT_1, 2)
              + readBit(BAG_SIZE_ADJUST_BIT_2, 4);
    }

    return DEFAULT_BAG_SIZE + ((sign * delta) * 10L);

}

long calculateActuatorSpeed() {

    pinMode(ACTUATOR_SPEED_ADJUST_ENABLE, INPUT_PULLUP);
    pinMode(ACTUATOR_SPEED_SIGN, INPUT_PULLUP);
    pinMode(ACTUATOR_SPEED_ADJUST_BIT_0, INPUT_PULLUP);
    pinMode(ACTUATOR_SPEED_ADJUST_BIT_1, INPUT_PULLUP);
    pinMode(ACTUATOR_SPEED_ADJUST_BIT_2, INPUT_PULLUP);

    long sign = 1;
    long delta = 0;

    if (digitalRead(ACTUATOR_SPEED_ADJUST_ENABLE) == 0) {
        sign = digitalRead(ACTUATOR_SPEED_SIGN) == 0 ? -1 : 1;
        delta = readBit(ACTUATOR_SPEED_ADJUST_BIT_0, 1)
              + readBit(ACTUATOR_SPEED_ADJUST_BIT_1, 2)
              + readBit(ACTUATOR_SPEED_ADJUST_BIT_2, 4);
    }

    return DEFAULT_ACTUATOR_SPEED + ((sign * delta) * 2L);

}


/**
 * =====================================================
 * Actuator controller and breathing cycle functionality
 * =====================================================
 */

// Comment out all but one of the following based on the board being used
#define ADAFRUIT_MOTOR_SHIELD
#define BARE_L239D

OneShotExecutionHandler breathingCycleManager = OneShotExecutionHandler("breathingCycle");

// The actuator controller
#ifdef ADAFRUIT_MOTOR_SHIELD
#include "AdafruitMotorShield.h"
AdafruitMotorShield actuatorController = AdafruitMotorShield();
#endif

#ifdef BARE_L239D
#include "L239D.h"
L239D actuatorController = L239D(BARE_L239D_CONTROL_PIN_1, BARE_L239D_CONTROL_PIN_2);
#endif

// The current setting for the number of breaths per minute
long breathsPerMinute;

// The size of the breath (percentage of bag size)
long breathSize;

// The size of the bag in millimeters
long bagSize;

// The actuator speed in millimeters per second
long actuatorSpeed;

// The timings for each of the four phases of the compression cycle, calculated at the start of each cycle.
unsigned long timeToExtendInMillis;
unsigned long timeToWaitExtendedInMillis;
unsigned long timeToRetractInMillis;
unsigned long timeToWaitRetractedInMillis;

// The variables that track which phase of the compression cycle we are in
boolean moving;
boolean compressing;
boolean retracting;

void stopRetraction() {

    moving = false;
    actuatorController.stop();
    breathingCycleManager.begin(timeToWaitRetractedInMillis);

}

void startRetraction() {

    retracting = true;
    compressing = false;
    moving = true;
    actuatorController.run(RETRACT);
    breathingCycleManager.begin(timeToRetractInMillis);

}

void stopCompression() {

    moving = false;
    actuatorController.stop();
    breathingCycleManager.begin(timeToWaitExtendedInMillis);

}

void startCompression() {

    // distance to move in mm - breathSize is the percentage of the bagSize
    long distanceToMove = bagSize * breathSize / 100L;

    // how long the cycle must last for in milliseconds
    long cycleTime = ((60 * 1000L) / breathsPerMinute);

    // how much time allowed to extend in milliseconds
    timeToExtendInMillis = distanceToMove * 1000L / actuatorSpeed;
    if (timeToExtendInMillis > (cycleTime / 2L)) {

#ifdef DEBUG
        Serial.println("Error: time to extend > 50% cycle time");
#endif

        timeToExtendInMillis = (cycleTime / 2L);
    }

    // how much time allowed to extend in milliseconds
    timeToWaitExtendedInMillis = (cycleTime / 2L) - timeToExtendInMillis;
    timeToRetractInMillis = timeToExtendInMillis + 100L;
    if (timeToRetractInMillis > (cycleTime / 2L)) {
        timeToRetractInMillis = cycleTime / 2L;
    }
    timeToWaitRetractedInMillis = (cycleTime / 2L) - timeToRetractInMillis;

#ifdef DEBUG_TIMINGS
    Serial.println("Cycle timings:");
    Serial.print("  breaths per minute:     ");
    Serial.println(breathsPerMinute);
    Serial.print("  cycle time:             ");
    Serial.println(cycleTime);
    Serial.print("  distance to move:       ");
    Serial.println(distanceToMove);
    Serial.print("  time to extend:         ");
    Serial.println(timeToExtendInMillis);
    Serial.print("  time to wait extended:  ");
    Serial.println(timeToWaitExtendedInMillis);
    Serial.print("  time to retract:        ");
    Serial.println(timeToRetractInMillis);
    Serial.print("  time to wait retracted: ");
    Serial.println(timeToWaitRetractedInMillis);
#endif

    retracting = false;
    compressing = true;
    moving = true;
    actuatorController.run(EXTEND);
    breathingCycleManager.begin(timeToExtendInMillis);

}

void advanceCycle() {
    if (compressing) {
        if (moving) {
            stopCompression();
        }
        else {
            startRetraction();
        }
    }
    else { // retracting
        if (moving) {
            stopRetraction();
        }
        else {
            startCompression();
        }
    }
}


/**
 * ==============================
 * Settings manager functionality
 * ==============================
 *
 * We periodically read the user controls (breathing rate and breath size analogue inputs) and
 * convert them into real world values - for display, and to be used whn the next compression
 * cycle starts.
 */

AsynchronousExecutionHandler settingsRefreshManager = AsynchronousExecutionHandler("settings");

long scaleValue(long minValue, long maxValue, long setting) {
   long range = maxValue - minValue;
   long settingScaledToRange = ((setting * range) / 1023L) + minValue;
   if (settingScaledToRange < minValue) {
       settingScaledToRange = minValue;
   }
   if (settingScaledToRange > maxValue) {
       settingScaledToRange = maxValue;
   }
   return settingScaledToRange;
}

long calculateBreathSize() {
    return scaleValue(MIN_BREATH_SIZE, MAX_BREATH_SIZE, analogRead(BREATH_SIZE));
}

long calculateBreathsPerMinute() {
    return scaleValue(MIN_BREATHS_PER_MINUTE, MAX_BREATHS_PER_MINUTE, analogRead(BREATHS_PER_MINUTE));
}

void updateSettings() {
    breathSize = calculateBreathSize();
    breathsPerMinute = calculateBreathsPerMinute();
}


/**
 * ============================
 * Display update functionality
 * ============================
 */

#define STATE_SETUP     0
#define STATE_RUNNING   1

LiquidCrystal _lcd = LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Lcd16x2 display = Lcd16x2(&_lcd);
AsynchronousExecutionHandler displayRefreshManager = AsynchronousExecutionHandler("display");

// Used to indicate if the system is currently resetting (false) or is running (true)
boolean running;

// Used to calculate how long the system has been running for
unsigned long startTime;

/**
 * Update the display
 */
void updateDisplay() {
    unsigned long runTime = millis() - startTime;
    display.update(breathsPerMinute, breathSize, actuatorSpeed, bagSize, running, runTime);
}


/**
 * ==========================
 * Device reset functionality
 * ==========================
 *
 * At power up we do not know the actuator position. Therefore the first thing we do is to run
 * the actuator controller to fully retract the actuator. We run it for ten seconds which is
 * far more than should be needed. The actuator is required to have an end stop mechanism to
 * prevent things from getting damaged when the actuator is fully retracted but the actuator
 * motor is still running.
 */

OneShotExecutionHandler resetPhaseManager = OneShotExecutionHandler("reset");

/**
 * Start the actuator retracting
 */
void startResetPhase() {

#ifdef DEBUG
    Serial.println("Reset phase starting");
#endif

    running = false;
    unsigned long resetTime = 10 * 1000L;
    actuatorController.run(RETRACT);
    resetPhaseManager.begin(resetTime);

}

/**
 * Stop the actuator and start the system running.
 */
void finishResetPhase() {

    actuatorController.stop();

#ifdef DEBUG
    Serial.println("Reset phase completed");
#endif

    startRunPhase();

}

void startRunPhase() {

    startTime = millis();
    running = true;

#ifdef DEBUG
    Serial.println("Run phase started");
#endif

    startCompression();

}


/**
 * ============================
 * Clock rollover functionality
 * ============================
 */

unsigned long savedSystemTime = 0;

/**
 * If the clock rolls over then the current system time will be less than the last saved system time
 */
boolean isClockRolledOver() {

    unsigned long currentSystemTime = millis();
    boolean rolledOver = currentSystemTime < savedSystemTime;
    savedSystemTime = currentSystemTime;

    return rolledOver;

}

/**
 * If the clock rolls over this could disrupt event scheduling, if an event hasn't been triggered and has a scheduled time close to (just before) the rollover.
 * Therefore we check for rollover and, if it happens, take action:
 *
 * First, cancel the scheduled event that would happen at the end of the current part of the cycle.
 * Second, decide what to do next:
 * - If we are in the compression half of the cycle:
 *   - Trigger the startRetraction state, regardless of if the actuator is currently moving.
 *   - This has the effect of slightly shortening the compression phase for one cycle.
 * - If we are in the retraction half of the cycle:
 *   - If we are moving the actuator:
 *     - Set the time to wait whilst retracted to zero.
 *     - Trigger the startRetraction state again to ensure we fully retract the actuator before the next cycle.
 *     - This may still lengthen the retraction phase slightly.
 *   - If we are not moving the actuator:
 *     - Trigger the startCompression state.
 *     - This has the effect of slightly shortening the retraction phase for one cycle.
 *
 * Note: If the calculated next execution time rolled over there might not be a problem. But better safe than sorry.
 */

void handleClockRollover() {

    breathingCycleManager.cancel();
    if (compressing) {
        startRetraction();
    }
    else if (retracting) {
        if (moving) {
            timeToWaitRetractedInMillis = 0L;
            startRetraction();
        }
        else {
            startCompression();
        }
    }
    startTime = 0; // otherwise the runtime will be garbage

}


/**
 * ====================
 * Arduino entry points
 * ====================
 */

void setup() {

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("setup() started");
#endif

    // Configure the attached display and actuator controller.
    display.begin();
    actuatorController.begin();

    // Calculate the bag size and actuator speed.
    bagSize = calculateBagSize();
    actuatorSpeed = calculateActuatorSpeed();

    // Start the managers that periodically read the settings and update the display.
    settingsRefreshManager.begin(500);
    displayRefreshManager.begin(500);

    // Start resetting the system.
    startResetPhase();

#ifdef DEBUG
    Serial.println("setup() completed");
#endif

}

void loop() {

    if (resetPhaseManager.isDue()) finishResetPhase();
    if (breathingCycleManager.isDue()) advanceCycle();
    if (settingsRefreshManager.isDue()) updateSettings();
    if (displayRefreshManager.isDue()) updateDisplay();
    if (isClockRolledOver()) handleClockRollover();

    // We wait for 2ms at the end of each loop. This is primarily to ensure that we detect clock rollover.
    delay(2);

}
