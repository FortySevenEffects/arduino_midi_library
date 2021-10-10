#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
    pinMode(2, INPUT);
    
    MIDI // chaining MIDI commands - order is from top to bottom (turnThruOff,... begin)
        .turnThruOff()
        // using a lamdba function for this callbacks
        .setHandleNoteOn([](byte channel, byte note, byte velocity)
            {
                // Do whatever you want when a note is pressed.

                // Try to keep your callbacks short (no delays ect)
                // otherwise it would slow down the loop() and have a bad impact
                // on real-time performance.
            })
        .setHandleNoteOff([](byte channel, byte note, byte velocity)
            {
                // Do something when the note is released.
                // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
            })
        .begin(MIDI_CHANNEL_OMNI); // Initiate MIDI communications, listen to all channels
}

void loop()
{
    // Call MIDI.read the fastest you can for real-time performance.
    MIDI.read();

    if (digitalRead(2))
        MIDI // chained sendNoteOn commands
            .sendNoteOn(42, 127, 1)
            .sendNoteOn(40, 54, 1);
}
