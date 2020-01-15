/* 
   Rotary Encoder with Switch Example

    Copyright (c) 2019 by Ilias Iliopoulos info@fryktoria.com

    This file is part of "FR_RotaryEncoder".

    Licensed under GNU General Public License, version 3 of the License
      https://www.gnu.org/licenses/  
   

   Uses interrupts.
   Demonstrates the usage of longPress switch functionality, alongside
   the interrupts. This can be used if immediate response to key press for On/Off is
   necessary, together with a long press functionality. 
   On and Off, as well as the rotation, are handled by the interrupts
   and long press by the polling. 
   
   *** The long press functionality cannot be used in this method.
   
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
  Serial.println("Rotary Encoder example with interrupts.");

  // KY-040 does not have a pullup for the switch
  // so we must set one 
  rencoder.enableInternalSwitchPullup(); // Also activates setSwitchLogic(false);
  Serial.println("Enabled internal pullup resistor for the switch");

  rencoder.setRotaryLimits(rotaryMinimum, rotaryMaximum, rotaryWrapMode);
  
  // One interrupt is required for the rotary 
	attachInterrupt(digitalPinToInterrupt(pinSCK), ISRrotary, CHANGE);
 
  // A different interrupt is required for the switch. Pulse edge is set 
  // to CHANGE because the library design allows both edges
  attachInterrupt(digitalPinToInterrupt(pinSW), ISRswitch, CHANGE);

  //*************** Set the long press time to 2 seconds
  rencoder.setLongPressTime(2000);

}

// Interrupt handling routine for the rotary
void ISRrotary() {
   rencoder.rotaryUpdate();
}

// Interrupt handling routine for the switch
void ISRswitch() {
  rencoder.switchUpdate();
}

void loop() {

  //********* To enable polling of only the switches
  rencoder.switchUpdate();

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
