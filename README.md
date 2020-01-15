# FR_RotaryEncoder
An Arduino libray for Rotary Encoders (types KY-040/EC11 or similar)

    "FR_RotaryEncoder" is a library for using mechanical rotary encoders
    with a built-in push switch.
    
    Copyright (c) 2019 by Ilias Iliopoulos info@fryktoria.com

    This file is part of "FR_RotaryEncoder".

    Licensed under GNU General Public License, version 3 of the License
      https://www.gnu.org/licenses/  

    The part related to the debouncing of the rotary encoder is based on
    an idea documented in http://www.technoblogy.com/list?28Y4 
    The article and code is copyrighted as:

      David Johnson-Davies - www.technoblogy.com - 7th July 2018
      Arduino/Genuino Uno   
      CC BY 4.0
      Licensed under a Creative Commons Attribution 4.0 International license: 
      http://creativecommons.org/licenses/by/4.0/  

  =============================================================================

The code supports:

   * Operation via interrupts or polling
   * Debouncing of the rotary encoder rotational movement with a method that 
      does not utilize debouncing time, based on the work of David Johnson-Davies
   * Reading of the switch state and identification of Long Press
   * Setting of rotational sensitivity

  =============================================================================

  This code is implemented with the Arduino IDE and has been tested with 
  IDE 1.8.10, on an Arduino Nano.  

  The code has been tested with a KY-040 rotary encoder module, having pull-up 
  resistors at pins A and B and no pull-up at pin SW, such as:
  https://www.aliexpress.com/item/32648520888.html?spm=a2g0s.9042311.0.0.27424c4dMoxkCX 
 
  =============================================================================

#  Usage:

0. Include the library in your sketch

      ```c++
      #include "FR_RotaryEncoder.h"
      ```

1. Create an instance of the class and specify the pins used for A (SCK), B (DT) 
     and the push-button/switch (SW).
     It is important to set A to pin 2 or 3 which supports interrupts.
     Also important is to set SW to pin 2 or 3, whichever actually of the pins 2 and 3 
     is not connected to A.
     B can be any other Arduino I/O pin.
     The name `rencoder` below is arbitrary. You can name your class instance anyway you like.

      ```c++
      RotaryEncoder rencoder(pinSCK, pinDT, pinSW);
      ```

2. Check the wiring of the rotary encoder. By default, the Arduino inputs are
     configured **without** pull-ups. If the module has pull-up resistors 
     in pins A and B, you do not have to do anything. But if there are no pull-ups,
     you have to set the manually using the internal MCU pull-ups:

      ```c++
      rencoder.enableInternalRotaryPullups(); 
      ```

    If the module does not have a pull-up resistor for the switch, 
       you have to activate a pull-up resistor inside the Arduino. Use:

      ```c++
      rencoder.enableInternalSwitchPullup(); 
      ```

3. With pull-up resistors and a switch connecting to GND, the steady state (switch not pressed)
     is 1 and the active (switch pressed) is 0. If your hardware has a switch from pin to Vcc 
     and a pull down resistor,
     you can invert the logic with:

      ```c++
      rencoder.setSwitchLogic(true);
      ```

You may also use `rencoder.setRotaryLogic()` to invert the rotational direction.

4. Set the positive and negative limits of the rotational position, as well
     as the wrap-around mode, with:

      ```c++
      rencoder.setRotaryLimits(rotaryMinimum, rotaryMaximum, rotaryWrapMode);
      ```

    The **rotaryWrap** mode seems simple to understand but is a bit tricky.

    **false**: Increasing position will never be higher than the maximum value.
              Decreasing values cannot be lower than the minimum value.

    The tricky part is when this mode is used along with `setRotationalStep()`
              and the step is set to a value different than 1. Suppose you have set the 
              limits to max=10 and min=-9 and the step to 2. In the counter-clockwise direction,
              the position may receive values 0, -2, -4, -6, -8 but the next step 
              would be -10 which is lower than min. If it was allowed to take the value -9,
              the clockwise direction would subsequently take values -7, -5, -3, -1, 1 etc. which
              is probably not desirable. Therefore, the lowest position will be -8,
              regardless of the fact that the min is even lower.

    **true**: After increasing over the maximum value, the position will be set to the minimum 
             value, regardless of the setting of the rotational step. Similarly for the other
             direction.

5. You can set any of the operating parameters using the methods documented in **FR_RotaryEncoder.h**

6. If you wish to use interrupts, you must create your interrupt handling routine(s) or Interrupt Service Routines (ISR), such as:

      ```c++
      // Interrupt handling routine for the rotary
      void ISRrotary() {
         rencoder.rotaryUpdate();
      }
      // Interrupt handling routine for the switch
      void ISRswitch() {
        rencoder.switchUpdate();
      }
      ```

     In `setup()` you must associate each interrupt pin with the 
        relevant interrupt handling routine, such as:


      ```c++
      attachInterrupt(digitalPinToInterrupt(pinSCK), ISRrotary, CHANGE);
      attachInterrupt(digitalPinToInterrupt(pinSW), ISRswitch, CHANGE);
      ```


     If you do not want to use interrupts, but use the polling method instead, please note
        that you are not limited to use pins 2 and 3. Any other Arduino I/O pin will do.

     In such a case, you must place in the `loop()` the routines that handle the encoder. 
        If you only use the rotary part and not the switch, you can use:

      ```c++
      rencoder.rotaryUpdate();
      ```

     If you only use the switch, use:

      ```c++
      rencoder.switchUpdate();
      ```

     If you use both, you can do  both jobs at the same time by simply using:

      ```c++
      rencoder.update();
      ```

7. If you wish to use the **Long Press** functionality, in order to identify if the 
       switch has been pressed continouously, or to retrieve the time that the switch 
       remains pressed with `rencoder.keyPressedTime()`, you need to place `rencoder.switchUpdate()`
       or `rencoder.update()` in the loop, **even if your code uses interrupts**. It
       becomes questionable, of course, why you should continue to use such a 
       mixed interrupt/polling solution, but every design is different and, in any case,
       the library supports it. 

The library is accompanied with lots of examples which demonstrate most of the library
  features.   
  
For any issues, you can contact me at <mailto://info@fryktoria.com>       

