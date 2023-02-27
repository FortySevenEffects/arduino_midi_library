#include <MIDI.h>

// Disable the default Use1ByteParsing
struct CustomMIDISettings : public MIDI_NAMESPACE::DefaultSettings {
  static const bool Use1ByteParsing = false;
};

#if defined(ARDUINO_SAM_DUE) || defined(USBCON) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    // Leonardo, Due and other USB boards use Serial1 by default.
    MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1,  MIDI, CustomMIDISettings)
#else
    MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial,  MIDI, CustomMIDISettings)
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  if (MIDI.read())                    // If we have received a message
  {
    digitalWrite(LED_BUILTIN, HIGH);
    MIDI.sendNoteOn(42, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
    delay(1000);                // Wait for a second
    MIDI.sendNoteOff(42, 0, 1);     // Stop the note
    digitalWrite(LED_BUILTIN, LOW);
  }
}
