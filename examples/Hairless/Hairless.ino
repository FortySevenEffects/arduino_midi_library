#include <MIDI.h>
USING_NAMESPACE_MIDI

struct MySerialSettings : public MIDI_NAMESPACE::DefaultSerialSettings
{
  static const long BaudRate = 115200;
};

unsigned long t1 = millis();

MIDI_NAMESPACE::SerialMIDI<HardwareSerial, MySerialSettings> serialMIDI(Serial1);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial, MySerialSettings>> MIDI((MIDI_NAMESPACE::SerialMIDI<HardwareSerial, MySerialSettings>&)serialMIDI);

void setup()
{
  MIDI.begin(1);
}

void loop()
{
  MIDI.read();

  // send a note every second
  if ((millis() - t1) > 1000)
  {
    t1 = millis();

    MIDI.sendNoteOn(random(1, 127), 55, 1);
  }
}
