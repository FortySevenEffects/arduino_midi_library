#include <MIDI.h>

// This example shows how to create two instances of the library to create a merger.
// There are two MIDI couples of IO, A and B, each using thru and merging with the
// input from the other node. The result is the following:
// A out = A in + B in
// B out = B in + A in

#ifdef ARDUINO_SAM_DUE
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial,     midiA);
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,    midiB);
#elif defined(ARDUINO_SAMD_ZERO)
    MIDI_CREATE_INSTANCE(HardwareSerial, SerialUSB,  midiA);
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,    midiB);
#else
    #include <SoftwareSerial.h>
    SoftwareSerial softSerial(2,3);
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial,     midiA);
    MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, midiB);
#endif

void setup()
{
    // Initiate MIDI communications, listen to all channels
    midiA.begin(MIDI_CHANNEL_OMNI);
    midiB.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
    if (midiA.read())
    {
        // Thru on A has already pushed the input message to out A.
        // Forward the message to out B as well.
        midiB.send(midiA.getType(),
                   midiA.getData1(),
                   midiA.getData2(),
                   midiA.getChannel());
    }

    if (midiB.read())
    {
        // Thru on B has already pushed the input message to out B.
        // Forward the message to out A as well.
        midiA.send(midiB.getType(),
                   midiB.getData1(),
                   midiB.getData2(),
                   midiB.getChannel());
    }
}
