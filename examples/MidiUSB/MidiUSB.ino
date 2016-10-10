#include <MIDI.h>
#include <midi_UsbTransport.h>

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);

// --

void handleNoteOn(byte inChannel, byte inNumber, byte inVelocity)
{
    Serial.print("NoteOn  ");
    Serial.print(inNumber);
    Serial.print("\tvelocity: ");
    Serial.println(inVelocity);
}
void handleNoteOff(byte inChannel, byte inNumber, byte inVelocity)
{
    Serial.print("NoteOff ");
    Serial.print(inNumber);
    Serial.print("\tvelocity: ");
    Serial.println(inVelocity);
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    MIDI.begin();
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    Serial.println("Arduino ready.");
}

void loop() {
    MIDI.read();
}