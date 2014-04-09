#include <MIDI.h>

// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html

void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
    // Do whatever you want when you receive a Note On.

    if (velocity == 0)
    {
        // This acts like a NoteOff. You can ask the library to call the NoteOff
        // callback when receiving null-velocity NoteOn messages.
        // See MIDI_HANDLE_NULL_VELOCITY_NOTE_ON_AS_NOTE_OFF in midi_Settings.h
    }

    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.
}

// -----------------------------------------------------------------------------

void setup()
{
    // Connect the HandleNoteOn function to the library,
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
    // Call MIDI.read the fastest you can for real-time performance.
    MIDI.read();

    // There is no need to check if there are messages incoming
    // if they are bound to a Callback function.
    // The attached method will be called automatically
    // when the corresponding message has been received.
}
