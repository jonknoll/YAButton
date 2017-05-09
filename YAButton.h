/***************************************************************************//**
MIT License

Copyright (c) 2017 Jon Knoll

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@description Yet Another Button Library
There are many other Arduino Libraries out there that do button debouncing, but
at the time of creating this library I could not find one that worked on an
external polling system (so it can be run as a task) and handled button press
callbacks. This library does both.

Features:
- customizable active state: HIGH OR LOW (default is LOW to take advantage of
  the internal pull-up)
- digital input is initialized when button is instantiated (set to INPUT if
  active state is HIGH, and INPUT_PULLUP if active state is low)
- external control of the polling period (so it can be run as a periodic task)
- register callback functions for button press, long press and release
- customizable long press (wait for long press and repeated call while button
  is being held)

Usage:
First decide how often you want the button routine to run. Try 10ms or 15ms. You
probably want 2 or 3 polling periods to ensure you don't catch any bounce.
Typical bounce time is 10ms to 50ms, depending on the button. In the example I
assume about 30ms of debounce is required, so I set my polling period to 15ms
and the debounce to 30ms/15ms = 2 polling periods. Keep in mind that if your
debounce time is set to longer than 50ms, you may find the buttons occasionally
miss consecutive presses. You may have to experiment a bit.

Next decide that you are going to do when the button is pressed, released and
long pressed. This is done with callback functions. You register a callback
function with the set methods. When that action occurs, the function will be
called, to do whatever you want. The long press can either repeat the call
or only call once. You only need to provide callback functions for the actions
you want to handle.

* For no long press call -- don't provide a callback
* For long press calls -- provide a callback, and provide a delay to wait (in
   poll periods). If the pollTime is 0 then the long press callback will be
   called right after the (short) button press.
   all).
* For continuous long press calls -- provide a callback, a pollTime and a
   repeatPollTime. With the repeatPollTime you can decide how often you want
   the long press function to be called. This is handy for things like setting
   the clock time or scrolling quickly though a list of options.


See ButtonExample.ino for an example of how the button library should be used.

*******************************************************************************/
#ifndef YABUTTON_H
#define YABUTTON_H


#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   BUTTON_NOT_PRESSED = 0,
   BUTTON_DEBOUNCING,
   BUTTON_PRESSED,
   BUTTON_LONG_PRESSED,
   BUTTON_RELEASE_DEBOUNCING
} buttonStates_t;

typedef void (*buttonFunction)(void);

class YAButton
{
public:
   /**
    * Instantiate a button
    * 
    * @param pin - the digital pin number
    * @param debounceTime - the number of poll periods before the button is
    *    done bouncing.
    * @param activeState - (optional) the active state of the button. Default
    *    is active LOW (when the pin is 0V then the button is "pressed"). The
    *    default is low so you can use the internal pull-up resistor on the pin.
    */
   YAButton(uint8_t pin, uint8_t debounceTime, bool activeState=LOW);
   
   /**
    * Set the callback function for when the button is pressed (optional).
    * 
    * @param callbackFunction - the function that will be called when the button
    *    is pressed. The function should take no parameters and return nothing.
    */
   void setPressCallback(buttonFunction callbackFunction);
   
   /**
    * Set the callback function for when the button is long pressed (optional).
    * 
    * @param callbackFunction - the function that will be called when the button
    *    is long pressed. The function should take no parameters and return
    *    nothing.
    * @param pollTime - the number of polling periods to wait after the initial
    *    button press before the callback function is called.
    * @param repeatPollTime - (optional) the number of polling periods before
    *    repeated calls of the callback function. To only call the function 1
    *    time, leave this parameter as 0.
    */
   void setLongPressCallback(buttonFunction callbackFunction, uint8_t pollTime, uint8_t repeatPollTime=0);
   
   /**
    * Set the callback function for when the button released (optional).
    * 
    * @param callbackFunction - the function that will be called when the button
    *    is released. The function should take no parameters and return nothing.
    */
   void setReleaseCallback(buttonFunction callbackFunction);
   
   /**
    * The run method. Call this method every poll period.
    * NOTE: The poll period must be provided outside this library to ensure that
    * the run() method is called on a consistent basis.
    */
   void run();
   
private:
   uint8_t pinNumber;
   bool activeState;
   uint8_t debounceTime;
   buttonFunction buttonPressCallbackFunc;
   buttonFunction buttonLongPressCallbackFunc;
   uint8_t longPressPollTime;
   uint8_t longPressRepeatTime;
   buttonFunction buttonReleaseCallbackFunc;
   uint8_t counter;
   uint8_t timeout;
   int prevPinState;
   buttonStates_t buttonState;
};


#ifdef __cplusplus
}
#endif

#endif
