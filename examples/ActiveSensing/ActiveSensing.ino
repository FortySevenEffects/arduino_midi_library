#include <MIDI.h>

///

struct MySettings : public MIDI_NAMESPACE::DefaultSettings
{
    static const bool UseSenderActiveSensing = true;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI, MySettings);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  MIDI.begin(1);                      // Launch MIDI and listen to channel 4
}

void loop()
{
  if (MIDI.read())                    // If we have received a message
  {
    digitalWrite(LED_BUILTIN, HIGH);
    MIDI.sendNoteOn(42, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
    delay(1000);		            // Wait for a second
    MIDI.sendNoteOff(42, 0, 1);     // Stop the note
    digitalWrite(LED_BUILTIN, LOW);
  }
}
