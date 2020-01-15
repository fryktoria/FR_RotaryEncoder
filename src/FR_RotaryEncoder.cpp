/*
  =============================================================================
    FR_RotaryEncoder.cpp
  =============================================================================

    "FR_RotaryEncoder" is a library for using mechanical rotary encoders
    with a built-in push switch.
    
    Copyright (c) 2019 by Ilias Iliopoulos info@fryktoria.com

    This file is part of "FR_RotaryEncoder".

    "FR_RotaryEncoder" is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License, or (at your option) 
    any later version.

    "FR_RotaryEncoder" is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FR_RotaryEncoder.  If not, see <https://www.gnu.org/licenses/>.


    The part related to the debouncing of the rotary encoder is based on
    an idea documented in http://www.technoblogy.com/list?28Y4 
    The article and code is copyrighted as:

      David Johnson-Davies - www.technoblogy.com - 7th July 2018
      Arduino/Genuino Uno  
      CC BY 4.0
      Licensed under a Creative Commons Attribution 4.0 International license: 
      http://creativecommons.org/licenses/by/4.0/  

  =============================================================================
*/


#include "FR_RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(int rotaryPinCLK, int rotaryPinDT, int switchPinSW)
{
	//Definitions
	pinA = rotaryPinCLK;
	pinB= rotaryPinDT;
    pinSwitch = switchPinSW;

	pinMode(pinA, INPUT);
	pinMode(pinB, INPUT);
    pinMode(pinSwitch, INPUT);
   
    // Internal usage is based on XOR and is opposite of user input 
    switchLogic = !(switchLogic);

    // Set the initial value to a0 and b0 as the current pin states
    a0 = digitalRead(pinA);
    b0 = digitalRead(pinB);
  
}

// ----- Rotary operations -----

void RotaryEncoder::setRotaryLimits(int rotaryMin, int rotaryMax, bool rotaryWrapMode) 
{
  minValue = rotaryMin;
  maxValue = rotaryMax;
  wrapMode = rotaryWrapMode;  
}

void RotaryEncoder::setRotaryLogic(bool logic) 
{
  rotaryLogic = (logic);
}

void RotaryEncoder::enableInternalRotaryPullups()
{
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
}

int RotaryEncoder::getDirection()
{
	return direction;
}

int RotaryEncoder::getPosition()
{
	return rotaryPosition;
}

void RotaryEncoder::setPosition(int newPosition)
{
	rotaryPosition = newPosition;
    // After an arbitrary set, the direction is ambiguous
    direction = NOT_MOVED;
}

void RotaryEncoder::setMaxValue(int newMaxValue)
{
	maxValue = newMaxValue;
}

void RotaryEncoder::setMinValue(int newMinValue)
{
	minValue = newMinValue;
}

void RotaryEncoder::setWrapMode(bool newWrapMode)
{
	wrapMode = newWrapMode;
}

void RotaryEncoder::setSensitive(bool fast)
{
	sensitive = fast;
}

void RotaryEncoder::setRotationalStep(int step)
{
	rotationalStep = step;
}

void RotaryEncoder::rotaryUpdate()
//Can be called from interrupt or loop
{
  int a = digitalRead(pinA);
  int b = digitalRead(pinB);
  if (a != a0) { 
    a0 = a;
    if (b0 == ROTARY_POSITION_UNKNOWN) {
      b0 = b;
      return;
    }
    if (b != b0) {
      //Serial.print("a= ");Serial.print(a); Serial.print(" b="); Serial.println(b);

      // Both signals A and B change at a 90 degree phase. Therefore, bouncing 
      // of one signal has (hopefully) ended when the other signal changes.
      // We now know that both a and b have changed, so we have a valid transition,
      // not suffering from bounce.

      // Now we check the current states of A and B to identify the rotation direction.
      changeRotaryValue(!(a == b));
      /*
      This is the same as:
      if (a) {
        // Rising edge of A, new a=1
        if (b) 
          changeRotaryValue(false); // a=1 and b=1, CCW
        else
          changeRotaryValue(true); // a=1 and b=0, CW
      } else {
        // Falling edge of A, new a is 0
        if (b)
          changeRotaryValue(true); // a=0 and b=1, CW
        else
          changeRotaryValue(false); // a=0 and b=0, CCW
      }
      */

      if (sensitive) {
        //Serial.println("Fast");
        b0 = b;
      } else {
        // This makes previous B unknown, so the encoder always requires two steps to make one change
        //Serial.println("Slow");
        b0 = ROTARY_POSITION_UNKNOWN; 
      }
    }
  }
}

void RotaryEncoder::changeRotaryValue(bool leftRight)
{
  int nextRotaryPosition;

  leftRight ^= rotaryLogic;

  if (leftRight) {
    nextRotaryPosition = rotaryPosition + rotationalStep;
    direction = CW;
  } else {
    nextRotaryPosition = rotaryPosition - rotationalStep;
    direction = CCW;
  }

  if (wrapMode) {
    if (nextRotaryPosition > maxValue) 
      rotaryPosition = minValue;
    else if (nextRotaryPosition < minValue) 
      rotaryPosition = maxValue;
    else
      rotaryPosition = nextRotaryPosition;		
    
  } else {
    // Make sure that transitions remain within the range
    if ((nextRotaryPosition > maxValue) || (nextRotaryPosition < minValue)){
      // do not change position
      direction = NOT_MOVED;
    } else {
      rotaryPosition = nextRotaryPosition;
    }      
  }

//  Serial.print("Direction "); Serial.println(direction);
//  Serial.print("Position "); Serial.println(rotaryPosition);

}

// ----- Switch operations -----

void RotaryEncoder::enableInternalSwitchPullup()
{
  pinMode(pinSwitch, INPUT_PULLUP);
  // Defaults to inverted state
  setSwitchLogic(false);
}

void RotaryEncoder::setSwitchLogic(bool logic) 
{
  // A true input means that 1 is ON but in our implementation
  // we use the XOR function to the pin input,
  // so we invert to make it work as: input false -> invert, input true -> do not invert
  // which is the opposite of XOR
  switchLogic = !(logic);
}

void RotaryEncoder::setSwitchDebounceDelay(unsigned long dd) 
{
  debounceDelay = dd;
}

int RotaryEncoder::getSwitchState() 
{
  if (switchLongPress)
    return SW_LONG;
  else if (switchPressed)
    return SW_ON;
  else
    return SW_OFF; 
}

void RotaryEncoder::setLongPressTime(unsigned long longPress)
{
  longPressTime = longPress; 
}

bool RotaryEncoder::keyPressed() 
{
    return switchPressed; 
}

unsigned long RotaryEncoder::keyPressedTime()
{
  // ****** How shall it behave at millis wrap around??
  if (switchPressed)
    return (millis() - lastPressedTime);
  else
    return 0;
}

void RotaryEncoder::switchUpdate()
// We may come here either by an ISR caused by a rising or falling edge
// or during polling
{
  bool pinState = digitalRead(pinSwitch);
 
  // Apply the ON/OFF logic so that logic in the code below 1 is always true
  pinState ^= switchLogic; 

  if (switchPressed) {

    if ((millis() - lastPressedTime) > longPressTime) {
      switchLongPress = true;
    }

    if ((millis() - lastPressedTime) > debounceDelay) {
      // Debouncing period finished, so state is the current state of the switch
      // which we consider as stable.
      // We also consider that the debouncing period is smaller than the time between 
      // consecutive switch presses
      if (!pinState) {
        switchPressed = false;
        switchLongPress = false;
        lastPressedTime = 0;
      } 
    }
  } else {
    if (pinState) {
      // New period when switch is considered as pressed
      switchPressed = true; 
      lastPressedTime = millis();
    } else {
      switchPressed = false;
      switchLongPress = false; 
      lastPressedTime = 0; 
    } 
  }
}

void RotaryEncoder::update()
{
  rotaryUpdate();
  switchUpdate();
}


