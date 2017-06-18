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
First decide how often you want the button routine to run. This is your polling
period (pollPeriod). Generally you want to poll the button fairly often so you
detect the button press as quickly as possible. Try 10ms or 15ms.

Second you want to decide on a debounce period. Typical bounce time for a
button is 10ms to 50ms, depending on the button. In the example I assume 30ms
of debounce is required. Keep in mind that if your debounce time is set to
longer than 50ms, you may find the buttons occasionally miss consecutive
presses (because it is still debouncing the previous press). You may have to
experiment a bit.
NOTE: The debounce period needs to be a multiple of the poll period (the button
routine only runs on the polling period).

Third decide what you are going to do when the button is pressed, released and
long pressed. This is done with callback functions. You register a callback
function with the "set" methods. When that action occurs, the function will be
called, to do whatever you want. The long press can either repeat the call
or only call once.
NOTE: You only need to provide callback functions for the actions you want to
handle.

* For long press calls -- provide a callback, and provide a delay to wait (in
   milliseconds). If the longPressDelay is 0 then the long press callback will
   be called right after the (short) button press.
* For continuous long press calls -- provide a callback, a longPressDelay and a
   repeatDelay. With the repeatDelay you can decide how often you want
   the long press function to be called. This is handy for things like setting
   the clock time or scrolling quickly though a list of options.
   NOTE: longPressDelay and repeatDelay should be multiples of the polling
   period.


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
    * @param pollPeriod - the number of milliseconds between each call of the
    *    run() method (how often you are going to check the button).
    * @param debounceDelay - the number of milliseconds before the button is
    *    done bouncing.
    *    NOTE: This should be a multiple of the pollPeriod or 0 for no debounce.
    * @param activeState - (optional) the active state of the button. Default
    *    is active LOW (when the pin is 0V then the button is "pressed"). The
    *    default is low so you can use the internal pull-up resistor on the pin.
    */
   YAButton(uint8_t pin, uint16_t pollPeriod, uint16_t debounceDelay, bool activeState=LOW);
   
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
    * @param longPressDelay - the number of milliseconds to wait after the
    *    initial button press before the long press callback function is
    *    called. If you want the long press callback to run immediately when
    *    the button is pressed, set the longPressDelay value to 0.
    *    NOTE: This should be a multiple of pollPeriod (or 0).
    * @param repeatDelay - (optional) the number of milliseconds before
    *    repeated calls of the callback function. To only call the function 1
    *    time, leave this parameter as 0. If you want the function to be called
    *    repeatedly as fast as possible, set the parameter to the pollPeriod.
    *    NOTE: The repeatDelay should be a multiple of pollPeriod (or 0).
    */
   void setLongPressCallback(buttonFunction callbackFunction, uint16_t longPressDelay, uint16_t repeatDelay=0);
   
   /**
    * Set the callback function for when the button released (optional).
    * 
    * @param callbackFunction - the function that will be called when the button
    *    is released. The function should take no parameters and return nothing.
    */
   void setReleaseCallback(buttonFunction callbackFunction);
   
   /**
    * The run method. Set up your program to call this method once per polling
    * period.
    * NOTE: The poll period must be set up outside this library to ensure that
    * the run() method is called on a consistent basis.
    */
   void run();
   
private:
   uint8_t pinNumber;
   bool activeState;
   uint16_t pollTimeMs;
   uint16_t debounceTime;
   buttonFunction buttonPressCallbackFunc;
   buttonFunction buttonLongPressCallbackFunc;
   uint16_t longPressPollTime;
   uint16_t longPressRepeatTime;
   buttonFunction buttonReleaseCallbackFunc;
   uint16_t counter;
   uint16_t timeout;
   int prevPinState;
   buttonStates_t buttonState;
};


#ifdef __cplusplus
}
#endif

#endif
