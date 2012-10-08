
#include <SoftwareSerial.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

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
    setupMidiInstances();
    setupTesters();
}

// -----------------------------------------------------------------------------

void loop()
{
    launchTests();
}
