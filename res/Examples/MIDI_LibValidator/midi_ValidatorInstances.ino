
#include "midi_ValidatorInstances.h"

// -----------------------------------------------------------------------------

SoftwareSerial softSerial(2, 3);

#if defined(__AVR_ATmega32U4__) // Leonardo uses Serial1 as hardware serial.
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,       midiHW);
    MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
#else
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial,        midiHW);
    MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
#endif

// \todo Create instance for USB if available

// -----------------------------------------------------------------------------

template<class SerialClass>
void setupMidiInstance(MIDI_CLASS(SerialClass)& inInstance)
{
    inInstance.begin(MIDI_CHANNEL_OMNI);
    inInstance.turnThruOff();
}

// -----------------------------------------------------------------------------

void setupMidiInstances()
{
    setupMidiInstance<HardwareSerial>(midiHW);
    setupMidiInstance<SoftwareSerial>(midiSW);
}

