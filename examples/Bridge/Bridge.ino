#include <MIDI.h>
#include <USB-MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1,  MIDI1);
USBMIDI_CREATE_DEFAULT_INSTANCE();

void handleMessage1(TypedMidiInterface::MidiMessage message)
{
    MIDI.sendMessage(message);
}

void handleMessage(TypedMidiInterface::MidiMessage message)
{
    MIDI1.sendMessage(message);
}

// -----------------------------------------------------------------------------

void setup()
{
    MIDI.setHandleMessage(handleMessage);
    MIDI1.setHandleMessage(handleMessage1);

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI1.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
    // Call MIDI.read the fastest you can for real-time performance.
    MIDI.read();
    MIDI1.read();
}
