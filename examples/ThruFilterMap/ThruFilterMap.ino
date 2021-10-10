#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

/**
 * This example shows how to make MIDI processors.
 *
 * The `filter` function defines whether to forward an incoming
 * MIDI message to the output.
 *
 * The `map` function transforms the forwarded message before
 * it is sent, allowing to change things.
 *
 * Here we will transform NoteOn messages into Program Change,
 * allowing to use a keyboard to change patches on a MIDI device.
 */

bool filter(const MIDIMessage& message)
{
    if (message.type == midi::NoteOn)
    {
        // Only forward NoteOn messages
        return true;
    }
    return false;
}

MIDIMessage map(const MIDIMessage& message)
{
    // Make a copy of the message
    MIDIMessage output(message);
    if (message.type == midi::NoteOn)
    {
        output.type = midi::ProgramChange;
        output.data2 = 0; // Not needed in ProgramChange
    }
    return output;
}

void setup()
{
    MIDI.begin();
    MIDI.setThruFilter(filter);
    MIDI.setThruMap(map);
}

void loop()
{
    MIDI.read();
}
