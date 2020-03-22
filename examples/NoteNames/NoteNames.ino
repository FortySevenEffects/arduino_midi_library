#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// -----------------------------------------------------------------------------

// This function will print the name of th note played.
// (check the callback example for more details on callbacks)

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    Serial.print("Note On: ");
    Serial.println(MIDI_NAMESPACE::NoteValues[inNote]);
}

// -----------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Arduino Ready");

  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
}
