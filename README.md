# project-diy-bvm

Arduino code to control a bag valve mask based ventilator.
Developed on request to support [ebcore](https://www.ebcore.io/) prototype number three.

Visit https://github.com/surfsoft/project-diy-bvm for up-to-date documentation and code.

# Licencing
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License here:

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.



## Key parameters
The BVM has the following key parameters that influence hardware selection:
- Bag size
- Maximum breaths per minute 

### Bag size
The linear actuator must:
- have a stroke long enough to fully squeeze the bag, and
- be capable of exerting the necessary force to fully squeeze the bag.

### Maximum breaths per minute
The linear actuator must be able to move quickly enough to complete a maximum bag squeeze in the required amount of time. 
For example, at twenty breaths per minute, with a maximum bag squeeze distance of 100mm the actuator must be able to move 100mm out, and then back, in three seconds.

Ideally it would be faster than this to permit deceleration as it reaches its stopping point to be programmed in. 
This both minimises wear and tear and reduces noise/vibration.
However, this is also dependent on the electronics permitting this level of control with a plain DC electric motor.

If we take 20 breaths per minute as the upper limit
And the bag has a maximum squeeze distance of 'X'
Then the linear actuator must be able to move at an absolute minimum of 66% of 'X' in one second.
Ideally it would be able to move faster than this - 75-80% of 'X' in one second.

## General design notes
The linear actuator will require a power supply that is capable of delivering the maximum current required by the linear actuator, at the required voltage.
In addition the Arduino will require its own power supply (which may be 5v or 3.3v).          

## Overview of controls

The system has two potentiometers that are used to adjust the breaths per minute and the size of each breath, connected to analogue inputs.
- Analogue input A0 is used to adjust the breaths per minute - between 10 and 20 breaths
- Analogue input A1 is used to set the size of a breath - between 20% and 95% of the size of the bag

Changes to either of these inputs will be reflected immediately on the attached LCD display, 
and will be used at the start of the next breath cycle. 
The upper and lower limits are set in the software.

The speed of the actuator is defined in the software as 100mm per second,
which is the slowest acceptable speed for the bag size set in the software at 150mm.
This is important as this speed of travel is used to calculate how long to run the electric motor when extending the actuator.
This speed can be adjusted up or down through the following digital input pins:
- Pin 13: pull low to indicate that the speed is to be adjusted. If nothing is connected to this pin at all it will appear high.
- Pin 16: pull low to reduce speed, pull high to increase
- Pins 17-19: Used to represent a three bit value, LSB first, multiplied by two, to adjust the speed by

Examples (default speed 100 mm/s):
- Pin 13 low, pin 16 high, pins 17-19 are low (0), high (2), high (4): add 6x2=12 to the base speed => 112mm/s
- Pin 13 low, pin 16 high, pins 17-19 are high (1), low (0), high (4): subtract 5x2=10 from the base speed => 90mm/s

The distance between fully retracted and fully extended for the actuator is not required.
The software assumes that the bag is positioned such that it just touches the fully retracted actuator.
The software has a default size for the bag, set to 150mm. 
This can be adjusted (in 10mm increments) through the following digital input pins:
- Pin 6: pull low to indicate that the size is to be adjusted. If nothing is connected to this pin at all it will appear high.
- Pin 7: pull low to reduce size, pull high to increase
- Pins 8-10: Used to represent a three bit value, LSB first, to adjust the size by

Examples (default bag size 150mm):
- Pin 6 low, pin 7 high, pins 8-10 are low (0), low (0), high (4): add 40mm to the base bag size => 190mm
- Pin 6 low, pin 7 low, pins 8-10 are high (1), high (2), low (0): subtract 30mm from the base bag size => 120mm

  
## Operation 

On power up the system enters a reset phase:
- the breaths per minute, breath size, actuator speed and bag size are calculated and displayed. 
- The actuator will return to fully retracted. 
- The software will allow ten seconds for this to complete before starting the breath cycle process.

Ten seconds is also long enough to check that any adjustments to the actuator speed and bag size are not catastrophically wrong.
After the ten seconds are up the software goes into the breathing cycle.

The time taken to complete a breathing cycle is calculated at the start of each cycle:
- The breath size and squeeze rate are read.
- Together with the bag size and actuator speed they are used to calculate the timings for the full cycle.
- The code assumes that the speed of the actuator is fixed.
- Changes to breath size and rate settings mid-cycle will be displayed immediately but won't be applied until the next cycle starts.

The cycle is made up of two halves, compress and release:
- At the start of the compress half of the cycle the actuator extends at a fixed speed for long enough to compress the bag required amount. 
- It then stops in position for the remainder of the compress half of the cycle.
- At the start of the release half of the cycle the actuator retracts at a fixed speed.
- We allow an extra 100ms for retraction in case the electric motor runs more slowly in reverse.
- Again, the actuator stops in position for the remainder of the "release" half of the cycle.

This cycle is repeated indefinitely.
At all times the display will show:
- Breath rate
- Breath size
- Programmed bag size
- Programmed actuator speed
- Run time in HHH:MM:SS (the system will display "===RESET===" during the 10s reset phase)

IMPORTANT NOTE: 
Event timings are calculated using the Arduino internal clock which uses a 32 bit number to count milliseconds since power-up.
If powered up for long enough (on the 49th day), the internal clock 'rolls over'.
The software is equipped to detect this event and in this situation the breathing cycle that is in progress may be shortened, before returning to the programmed breathing rate and breath size.  

Main components:
- Bag and associated tubing
- Linear actuator with in-built end stop switches (these are very important)
- Arduino (practically any Arduino will do provided it is pin-compatible with a standard Arduino)
- LCD display
- Adafruit motor driver shield v2.3 or two or more L239D chips controlled directly
- 5v power supply for the Arduino
- 12v power supply (or whatever the linear actuator needs)

# Linear actuator driver options

The software currently supports two alternative motor driver options:
- Adafruit motor driver shield v2.3
- Two or more L239D chips controlled directly

To select the driver you want to use, find the pair of #define lines in BvmController.ino:
- \#define ADAFRUIT_MOTOR_SHIELD
- \#define BARE_L239D

Comment out the one that you don't want. 
The checked-in code has neither commented out, to ensure the code won't compile, forcing you to make an explicit choice.

Other options will be added over time (the next option will be a PWM controlled speed controller).

# Power delivery

Electric motors doing this kind of work will usually require a minimum of 3-4A to operate at sufficient speed.
Each half of an L239D can only supply a peak of 1.2A.
Therefore regardless of which driver circuit you are using it is entirely possible that you will need two or more in parallel to achieve the required power output:
- With the Adafruit shield wire all four channels in parallel to get a maximum of 4.8A (the code assumes that all four channels will be required). Before connecting them all together you should use a voltmeter to check the polarity of each connection. 
- If using the L239D directly there's a basic wiring diagram High-power-motor-control-circuit.svg - you'll need one chip for every 2.4A required.    

If during the reset cycle the actuator extends instead of retracting, power down and reverse the connections to the electric motor.

  
# Arduino pin assignments

A standard Arduino has:
- A pair of dedicated I2C pins
- Fourteen digital pins 0-13
- Six analogue pins 0-5 (which can also be addressed as digital pins)

Pin assignments are currently as follows:
- The Adafruit motor shield works over I2C which has its own dedicated pins
- The LCD uses digital pins D2-D5, D11 and D12 
- The two sets of adjustable parameters use pins D6-D10, D13, and D16-D19 
- The controls use analogue pins A0 and A1
- Digital pins D0 and D1 are used for direct control of two or more L239D chips.


# Questions, issues etc

Please ask questions and raise issues through the project issue tracker at https://github.com/surfsoft/project-diy-bvm/issues  
