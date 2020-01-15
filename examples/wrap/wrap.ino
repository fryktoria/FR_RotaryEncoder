/* 
   Rotary Encoder with Switch Example

    Copyright (c) 2019 by Ilias Iliopoulos info@fryktoria.com

    This file is part of "FR_RotaryEncoder".

    Licensed under GNU General Public License, version 3 of the License
      https://www.gnu.org/licenses/  
   

   Uses only polling, without any interrupts
   Demonstrates the rotation Wrap Mode. After rotating through maximum value, the position 
   wraps up to the minimum value.
   Try also the counter clockwise direction.
   Demonstrates alse the rotational step setting. Notice the potential problem when 
   one of the limits is not a multiple of the rotational step. 
   
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
bool rotaryWrapMode = true; // Set true to wrap up

//-----------------------------------------------------------------------------------

int lastSwitchState = 0;
int lastPosition;

RotaryEncoder rencoder(pinSCK, pinDT, pinSW);

void setup() {
  Serial.begin(115200);
  Serial.println("Rotary Encoder example with Long Press and without interrupts.");

  // KY-040 does not have a pullup for the switch
  // so we must set one 
  rencoder.enableInternalSwitchPullup(); // Also activates setSwitchLogic(false);
  Serial.println("Enabled internal pullup resistor for the switch");

  rencoder.setRotaryLimits(rotaryMinimum, rotaryMaximum, rotaryWrapMode);
  rencoder.setRotationalStep(2);
  
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
