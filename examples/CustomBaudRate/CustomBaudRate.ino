#include <MIDI.h>

// Override the default MIDI baudrate to
// a decoding program such as Hairless MIDI (set baudrate to 115200)
struct CustomSerialSettings : public MIDI_NAMESPACE::DefaultSerialSettings {
  static const long BaudRate = 115200;
};

#if defined(ARDUINO_SAM_DUE) || defined(USBCON) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    // Leonardo, Due and other USB boards use Serial1 by default.
    MIDI_CREATE_CUSTOMSERIAL_INSTANCE(HardwareSerial, Serial1,  MIDI, CustomSerialSettings)
#else
    MIDI_CREATE_CUSTOMSERIAL_INSTANCE(HardwareSerial, Serial,  MIDI, CustomSerialSettings)
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
