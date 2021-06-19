#include <MIDI.h>
USING_NAMESPACE_MIDI

struct MyMIDISettings : public MIDI_NAMESPACE::DefaultSettings
{
  // When setting UseSenderActiveSensing to true, MIDI.read() *must* be called
  // as often as possible (1000 / SenderActiveSensingPeriodicity per second). 
  //
  // setting UseSenderActiveSensing to true, adds 34 bytes of code.
  //
  // When using Active Sensing, call MIDI.read(); in the Arduino loop()
  //
  // from 'a' MIDI implementation manual: "Sent periodically"
  // In the example here, a NoteOn is send every 1000ms (1s), ActiveSensing is
  // send every 250ms after the last command.
  // Logging the command will look like this:
  //
  // ...
  // A.Sense  FE
  // A.Sense  FE
  // A.Sense  FE
  // NoteOn   90 04 37 [E-2]
  // A.Sense  FE
  // A.Sense  FE
  // A.Sense  FE
  // NoteOn   90 04 37 [E-2]
  // A.Sense  FE
  // A.Sense  FE
  // A.Sense  FE
  // NoteOn   90 04 37 [E-2]
  // ...

  static const bool UseSenderActiveSensing = true;

  static const uint16_t SenderActiveSensingPeriodicity = 250;
};

unsigned long t1 = millis();

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI, MyMIDISettings);

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
