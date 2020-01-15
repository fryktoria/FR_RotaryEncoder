/*
  =============================================================================
    FR_RotaryEncoder.h
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

#ifndef FR_ROTARYENCODER_H
#define FR_ROTARYENCODER_H

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include <WProgram.h> 
#endif

// Default Rotational limits. Can be set with setRotaryLimits() 
#define DEFAULT_ROTARY_MIN -10 
#define DEFAULT_ROTARY_MAX  10

// Rotary wrap-around mode
//   false  Do not wrap-around
//   true   Wrap-around at limits  
#define DEFAULT_WRAP_MODE false  // Do not wrap-around

// Set true to invert rotational direction
#define DEFAULT_ROTARY_LOGIC false

// Sets the sensitivity of rotation
//   false Two clicks are required per count
//   true  One click is required per count (with first-click problem)
#define DEFAULT_SENSITIVITY false 

// Boolean logic of the switch wiring
//   true means:  switch ON if pin is 1, OFF if pin is 0 
//   false means: switch OFF if pin is 1, ON if pin is 0 
// Usually, pull-up resistors are used and idle state is 1, so false must be set
#define DEFAULT_SWITCH_LOGIC false

// In milliseconds. Can be changed with setDebounceDelay()
// For the switch only. Not used for rotary.
#define DEFAULT_DEBOUNCE_DELAY 100

// In milliseconds. Can be changed with setLongPressTime()
#define DEFAULT_LONG_PRESS_TIME 700

//==========================================================================

class RotaryEncoder
{
public:
	// Constructor
	RotaryEncoder(int rotaryPinCLK, int rotaryPinDT, int switchPinSW);

    enum RotationDirection {
      CCW = -1,
      CW = 1,
      NOT_MOVED = 0,
      ROTARY_POSITION_UNKNOWN = 888 
    };

    enum SwitchState {
      SW_OFF  = 0, 
      SW_ON   = 1,
      SW_LONG = 2
    };

    // Sets the limits of the rotary encoder, as well as the wrap mode
    void setRotaryLimits(int rotaryMin, int rotaryMax, bool rotaryWrapMode);

    // Sets the rotary switch logic.
    // Used to invert the rotation direction
    void setRotaryLogic(bool logic); 

    // Enable internal pullups resistors for the rotary
    void enableInternalRotaryPullups(); 

	// Retrieves the rotation direction of the previous rotation 
    // See enum RotationDirection
    // CW = Clockwise 
    // CCW = Counter-clockwise 
    // NOT_MOVED = Not moved since initialization or setPosition()
	int getDirection();

	// Returns the current position of the rotary encoder
	int getPosition();

	// Sets a starting position
	void setPosition(int newPosition);

	// Sets the maximum value of the rotation counter
	void setMaxValue(int newMaxValue);

	// Sets the minimum value of the rotation counter
	void setMinValue(int newMinValue);

	// Sets the wrap mode, if rotating in same direction exceeds min or max
    //   true goes from maxValue to minValue
    //   false stays at maxValue or minValue, if continued in the same direction 
	void setWrapMode(bool newWrapMode);

    // Sets the sensitivity of rotation
    // false (default): Requires two clicks per transition
    // true: Requires one click per transition (except the first after setup 
    // which depends on the initial switch position) 
    void setSensitive(bool fast);

    // Sets the step that position changes in every transition
    void setRotationalStep(int step);

    // Updates only the encoder state.
    void rotaryUpdate();

    // Switch

    // Enable internal pullup resistor for the switch
    void enableInternalSwitchPullup();

    // Sets the switch logic.
    //  true means:  switch ON if pin is 1, OFF if pin is 0
    //  false means: switch OFF if pin is 1, ON if pin is 0
    void setSwitchLogic(bool logic);

    // Sets the switch debouncing time in milliseconds
    void setSwitchDebounceDelay(unsigned long dd);

    // Returns the state of the switch
    // See enum SwitchState
    //   SW_OFF OFF
    //   SW_ON ON
    //   SW_LONG Long press
    int getSwitchState(); 

    // Set the minimum time after which a switch press is considered a Long Press
    void setLongPressTime(unsigned long longPress);

    // Returns true while switch is pressed
    bool keyPressed();

    // Returns the time that the switch is pressed, in milliseconds
    unsigned long keyPressedTime();

	// Updates the states of the internal values, both for the rotary and the switch.
    // Can be called either from loop or from interrupt.
	void update();

    // Updates only the switch state.
    void switchUpdate();

    
private:
	
    // Rotary	
	int pinA, pinB;  // Pins used for the rotary encoder.     
    void changeRotaryValue(bool up);

    // Set in the interrupt service routine and therefore
    // should be volatile
	volatile int direction = NOT_MOVED;
	volatile int rotaryPosition = 0; 
	volatile int a0 = ROTARY_POSITION_UNKNOWN;
	volatile int b0 = ROTARY_POSITION_UNKNOWN;

    // Switch
    int pinSwitch;   // Pin used for the switch
    bool switchLogic = DEFAULT_SWITCH_LOGIC;  
    volatile bool switchPressed = false; 
    volatile bool switchLongPress = false;
    volatile unsigned long lastPressedTime = 0;  // the last time the switch has been pressed

    /*
     I hate to write get methods for each one of the following.
     If needed and no other issues are considered, move them to 
     the public area, although this is not considered
     proper programming practice.
    */ 
    // Rotary
    bool rotaryLogic = DEFAULT_ROTARY_LOGIC; 	    
	int maxValue = DEFAULT_ROTARY_MAX;
	int minValue = DEFAULT_ROTARY_MIN;
	bool wrapMode = DEFAULT_WRAP_MODE; 
    bool sensitive = DEFAULT_SENSITIVITY; // Two clicks per count
    int rotationalStep = 1; // Position changes by 1. Can be set with setRotationalStep
    // Switch
    unsigned long debounceDelay = DEFAULT_DEBOUNCE_DELAY; // Increase if the output bounces flickers        
    unsigned long longPressTime = DEFAULT_LONG_PRESS_TIME; // Time for the switch to be pressed so as to be considered a long press (milliseconds) 

    
};
#endif
