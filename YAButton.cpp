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

@description See header file for a description
*******************************************************************************/
#include "Arduino.h"
#include "YAButton.h"




YAButton::YAButton(uint8_t pin, uint8_t debounceTime, bool activeState)
{
   pinNumber = pin;
   YAButton::activeState = activeState;
   YAButton::debounceTime = debounceTime;
   buttonPressCallbackFunc = 0;
   buttonLongPressCallbackFunc = 0;
   longPressPollTime = 0;
   longPressRepeatTime = 0;
   buttonReleaseCallbackFunc = 0;
   counter = 0;
   timeout = 0;
   prevPinState = 0;
   buttonState = BUTTON_NOT_PRESSED;
   // set the pin
   if(activeState == LOW)
   {
      pinMode(pin, INPUT_PULLUP);
   }
   else
   {
      pinMode(pin, INPUT);
   }
}

void YAButton::setPressCallback(buttonFunction callbackFunction)
{
   buttonPressCallbackFunc = callbackFunction;
}

void YAButton::setLongPressCallback(buttonFunction callbackFunction, uint8_t pollTime, uint8_t repeatPollTime)
{
   buttonLongPressCallbackFunc = callbackFunction;
   longPressPollTime = pollTime;
   longPressRepeatTime = repeatPollTime;
}

void YAButton::setReleaseCallback(buttonFunction callbackFunction)
{
   buttonReleaseCallbackFunc = callbackFunction;
}

// Set your polling interval to say 10ms and then call this function every 10ms 
void YAButton::run()
{
   int pinState;
   bool runAgain = true;
   
   while(runAgain)
   {
      runAgain = false;
      switch(buttonState)
      {
         case BUTTON_NOT_PRESSED:
         default:
            pinState = digitalRead(pinNumber);
            if(pinState == activeState)
            {
               counter = 0;
               timeout = debounceTime;
               buttonState = BUTTON_DEBOUNCING;
               
               // call the callback on first detection of the button press
               if(buttonPressCallbackFunc != 0)
               {
                  buttonPressCallbackFunc();
               }
               runAgain = true;
            }
            break;
            
         case BUTTON_DEBOUNCING:
            if(counter >= timeout)
            {
               // set up for next state
               counter = 0;
               timeout = longPressPollTime;
               buttonState = BUTTON_PRESSED;
               runAgain = true;
            }
            else
            {
               counter++;
            }
            break;
            
         case BUTTON_PRESSED:
            pinState = digitalRead(pinNumber);
            if(pinState != activeState)
            {
               // set up for next state
               counter = 0;
               timeout = debounceTime;
               buttonState = BUTTON_RELEASE_DEBOUNCING;

               // call the callback on first detection of the button release
               if(buttonReleaseCallbackFunc != 0)
               {
                  buttonReleaseCallbackFunc();
               }
               runAgain = true;
            }
            else // still pressed
            {
               if(counter >= timeout)
               {
                  // set up for next state
                  counter = 0;
                  timeout = longPressRepeatTime;
                  buttonState = BUTTON_LONG_PRESSED;

                  // call the callback on first detection of the button long press
                  if(buttonLongPressCallbackFunc != 0)
                  {
                     buttonLongPressCallbackFunc();
                  }
                  runAgain = true;
               }
               else
               {
                  counter++;
               }
            }
            break;

         case BUTTON_LONG_PRESSED:
            pinState = digitalRead(pinNumber);
            if(pinState != activeState)
            {
               // set up for next state
               counter = 0;
               timeout = debounceTime;
               buttonState = BUTTON_RELEASE_DEBOUNCING;

               // call the callback on first detection of the button release
               if(buttonReleaseCallbackFunc != 0)
               {
                  buttonReleaseCallbackFunc();
               }
               runAgain = true;
            }
            else // still pressed
            {
               if(longPressRepeatTime > 0)
               {
                  if(counter >= timeout)
                  {
                     counter = 0;

                     // call the callback on first detection of the button long press
                     if(buttonLongPressCallbackFunc != 0)
                     {
                        buttonLongPressCallbackFunc();
                     }
                  }
                  else
                  {
                     counter++;
                  }
               }
            }
            break;
            
         case BUTTON_RELEASE_DEBOUNCING:
            if(counter >= timeout)
            {
               // set up for next state
               buttonState = BUTTON_NOT_PRESSED;
               runAgain = true;
            }
            else
            {
               counter++;
            }
            break;
      }
   }
}
