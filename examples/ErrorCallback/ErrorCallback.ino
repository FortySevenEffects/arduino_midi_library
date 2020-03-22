#include <MIDI.h>

// Simple tutorial on how to receive and send MIDI messages.
// Here, when receiving any message on channel 4, the Arduino
// will blink a led and play back a note for 1 second.

MIDI_CREATE_DEFAULT_INSTANCE();
USING_NAMESPACE_MIDI

void handleError(int8_t err)
{
  if (bitRead(err, ErrorActiveSensingTimeout))
    Serial.println("ActiveSensing Timeout");
  else
    Serial.println("ActiveSensing OK");

  digitalWrite(LED_BUILTIN, (err == 0)? LOW : HIGH);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("booting");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  MIDI.setHandleError(handleError);
  MIDI.begin(1);
}

void loop()
{
  MIDI.read();
}
