#include <Arduino.h>
#include "midi_ValidatorInstances.h"

// -----------------------------------------------------------------------------

SoftwareSerial softSerial(10, 11);

#if defined(__AVR_ATmega32U4__) // Leonardo uses Serial1 as hardware serial.
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,       midiHW);
    MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
#else
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial,        midiHW);
    MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
#endif

void setupMidi()
{
    while (!softSerial.isListening())
        softSerial.listen();
}
