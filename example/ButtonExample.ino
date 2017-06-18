/**
 * Button example. This uses a poll time of 10ms to run a button on pin 9.
 * It flashes the LED on pin 13.
 */

#include "Arduino.h"
#include "YAButton.h"

const int ledPin = 13;
const int buttonPin = 9;

const int pollTimeInMs = 10;
const int debounceTimeMs = 30;
boolean ledState = LOW;

YAButton button1(buttonPin, pollTimeInMs, debounceTimeMs);
uint32_t nextRun;

static void ledOn(void);
static void ledOff(void);
static void ledBlink(void);

//The setup function is called once at startup of the sketch
void setup()
{
   pinMode(ledPin, OUTPUT);
   nextRun = millis();
   button1.setPressCallback(ledOn);
   // Start blinking after button has been pressed for 2 seconds
   // Keep blinking at a rate of 2Hz as long as the button is pressed
   button1.setLongPressCallback(ledBlink, 2000, 250);
   button1.setReleaseCallback(ledOff);
}

// The loop function is called in an endless loop
void loop()
{
   uint32_t msecNow;
   
   msecNow = millis();
   if(msecNow >= nextRun)
   {
      nextRun = msecNow + pollTimeInMs;
      button1.run();
   }
}

static void ledOn(void)
{
   ledState = HIGH;
   digitalWrite(ledPin, ledState);
}

static void ledOff(void)
{
   ledState = LOW;
   digitalWrite(ledPin, ledState);
}

static void ledBlink(void)
{
   if(ledState == LOW)
   {
      ledState = HIGH;
   }
   else
   {
      ledState = LOW;
   }
   digitalWrite(ledPin, ledState);
}
