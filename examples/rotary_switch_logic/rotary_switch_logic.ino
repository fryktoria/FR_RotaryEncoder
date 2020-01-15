/* 
   Rotary Encoder with Switch Example

    Copyright (c) 2019 by Ilias Iliopoulos info@fryktoria.com

    This file is part of "FR_RotaryEncoder".

    Licensed under GNU General Public License, version 3 of the License
      https://www.gnu.org/licenses/  


   Uses only polling, without any interrupts.
   Demonstrates the control of the rotation direction with setRotaryLogic(). 
   Used to invert the indication of the direction of rotation

   Also demonstrates the use of setSwitchLogic(). Used to match switch wiring (normally On 
   or normally OFF) with the switch logic.

   Wiring of the KY-040 Rotary Encoder Module
   ------------------------------------------
   Arduino    KY-040
     2          SCK
     3          SW
     4          DT
     +5V        +
     GND        GND 
   

*/
 
#include "FR_RotaryEncoder.h"

int pinSCK = 2; // Interrupt pin for rotary encoder. Can be 2 or 3      
int pinDT = 4; // Better select a pin from 4 to 7
int pinSW = 3; // Interrupt pin for switch. Can be 2 or 3 
int rotaryMaximum = 10;
int rotaryMinimum = -(rotaryMaximum - 1);
bool rotaryWrapMode = false;

//-----------------------------------------------------------------------------------

int lastSwitchState = 0;
int lastPosition;

RotaryEncoder rencoder(pinSCK, pinDT, pinSW);

void setup() {
  Serial.begin(115200);
  Serial.println("Rotary Encoder example with Long Press and without interrupts.");

  rencoder.setRotaryLimits(rotaryMinimum, rotaryMaximum, rotaryWrapMode);

  //*********** Reverses the CW - CCW direction
  rencoder.setRotaryLogic(true);

  // **** Only because test is performed with a KY-040 and we need the internal pull-up resistor
  rencoder.enableInternalSwitchPullup(); // Also activates setSwitchLogic(false);

  //*********** Reverses ON - OFF switch state
  // When the switch is wired with a pull down resistor.
  // This method does not change the internal pull-up resistor setting
  rencoder.setSwitchLogic(true);  
  
}


void loop() {

  // Use update() both for switch and rotary
  rencoder.update();

  // Code to present state changes. Same in all examples.
  
  // Rotary 
  int currentPosition = rencoder.getPosition();
  if (lastPosition != currentPosition) {
    lastPosition = currentPosition;
    Serial.print(currentPosition); Serial.print(" ");
    int direction = rencoder.getDirection();
    printRotationalDirection(direction); 
    Serial.println();
  }

  // Switch
  int currentSwitchState = rencoder.getSwitchState();
  if (lastSwitchState != currentSwitchState) {
    lastSwitchState = currentSwitchState;
    if (currentSwitchState == rencoder.SW_LONG)
      Serial.println("Switch Long ON");
    else if (currentSwitchState == rencoder.SW_ON)        
      Serial.println("Switch ON");
    else
      Serial.println("Switch OFF");       
  }
}


void printRotationalDirection(int direction) {
  switch(direction) {
    case rencoder.CW:
      Serial.print("CW");
      break;
    case rencoder.CCW:
      Serial.print("CCW");
      break;
    case rencoder.NOT_MOVED:
      Serial.print("NOT_MOVED");
      break;
    default:
      Serial.print("Unrecognized direction of rotation");  
  } 
}
