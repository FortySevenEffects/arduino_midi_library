
#include <SoftwareSerial.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

#include <Arduino.h>
#include "midi_ValidatorSerialDebug.h"
#include "midi_ValidatorLCD.h"
#include "midi_ValidatorLEDs.h"
#include "midi_ValidatorInstances.h"
#include "midi_ValidatorTester.h"

// -----------------------------------------------------------------------------

void setup()
{
    setupSerialDebug();
    setupLCD();
    setupLEDs();
    setupTesters();
    Serial.println("Tester ready");
}

// -----------------------------------------------------------------------------

void loop()
{
    if (launchTests() == true)
    {
    	setLedsFinal(true);
    	Serial.println("All tests passed.");
    }
    else
    {
    	setLedsFinal(false);
    	Serial.println("Some tests failed!");
    }
    while (true);
}
