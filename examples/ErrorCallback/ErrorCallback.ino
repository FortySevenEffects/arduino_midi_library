#include <MIDI.h>

// Before running the program below, make sure you set
// UseReceiverActiveSensing (optionally UseSenderActiveSensing) in Settings.h to true

MIDI_CREATE_DEFAULT_INSTANCE();

void handleError(int8_t err)
{
  digitalWrite(LED_BUILTIN, (err == 0)? LOW : HIGH);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  MIDI.setHandleError(handleError);
  MIDI.begin(1);
}

void loop()
{
  MIDI.read();
}
