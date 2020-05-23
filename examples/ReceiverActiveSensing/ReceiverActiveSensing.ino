#include <MIDI.h>
USING_NAMESPACE_MIDI

struct MyMIDISettings : public MIDI_NAMESPACE::DefaultSettings
{
  // When setting UseReceiverActiveSensing to true, MIDI.read() *must* be called
  // as often as possible (1000 / SenderActiveSensingPeriodicity per second).
  //
  // setting UseReceiverActiveSensing to true, adds 174 bytes of code.
  //
  // (Taken from a Roland MIDI Implementation Owner's manual)
  // Once an Active Sensing message is received, the unit will begin monitoring
  // the interval between all subsequent messages. If there is an interval of 420 ms
  // or longer between messages while monitoring is active, the same processing
  // as when All Sound Off, All Notes Off,and Reset All Controllers messages are
  // received will be carried out. The unit will then stopmonitoring the message interval.

  static const bool UseReceiverActiveSensing = true;

  static const uint16_t ReceiverActiveSensingTimeout = 420;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI, MyMIDISettings);

void activeSensingTimeoutExceptionHandler(bool active)
{
  if (!active)
  {
    MIDI.sendControlChange(AllSoundOff, 0, 1);
    MIDI.sendControlChange(AllNotesOff, 0, 1);
    MIDI.sendControlChange(ResetAllControllers, 0, 1);

    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
    digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  MIDI.setHandleActiveSensingTimeout(activeSensingTimeoutExceptionHandler);
  MIDI.begin(1);
}

void loop()
{
  MIDI.read();
}
