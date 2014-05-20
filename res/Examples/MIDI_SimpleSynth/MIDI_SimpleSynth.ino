#include <MIDI.h>
#include "noteList.h"
#include "pitches.h"

#ifdef ARDUINO_SAM_DUE // Due has no tone function (yet), overriden to prevent build errors.
#define tone(...)
#define noTone(...)
#endif

// This example shows how to make a simple synth out of an Arduino, using the
// tone() function. It also outputs a gate signal for controlling external
// analog synth components (like envelopes).

static const unsigned sGatePin     = 13;
static const unsigned sAudioOutPin = 10;
static const unsigned sMaxNumNotes = 16;
MidiNoteList<sMaxNumNotes> midiNotes;

// -----------------------------------------------------------------------------

void handleGateChanged(bool inGateActive)
{
    digitalWrite(sGatePin, inGateActive ? HIGH : LOW);
}

void pulseGate()
{
    handleGateChanged(false);
    delay(1);
    handleGateChanged(true);
}

// -----------------------------------------------------------------------------

void handleNotesChanged()
{
    if (midiNotes.empty())
    {
        handleGateChanged(false);
        noTone(sAudioOutPin);
    }
    else
    {
        byte currentNote = 0;
        if (midiNotes.getTail(currentNote))
        {
            tone(sAudioOutPin, sNotePitches[currentNote]);
            pulseGate(); // Retrigger envelopes. Remove for legato effect.
        }
    }
}

// -----------------------------------------------------------------------------

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    midiNotes.add(MidiNote(inNote, inVelocity));
    handleNotesChanged();
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    midiNotes.remove(inNote);
    handleNotesChanged();
}

// -----------------------------------------------------------------------------

void setup()
{  
    pinMode(sGatePin,     OUTPUT);
    pinMode(sAudioOutPin, OUTPUT);
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin();
}

void loop()
{
    MIDI.read();
}
