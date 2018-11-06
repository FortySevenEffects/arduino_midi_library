#include <MIDI.h>

#if defined(USBCON)
#include <midi_UsbTransport.h>

static const unsigned sUsbTransportBufferSize = 16;
typedef midi::UsbTransport<sUsbTransportBufferSize> UsbTransport;

UsbTransport sUsbTransport;

MIDI_CREATE_INSTANCE(UsbTransport, sUsbTransport, MIDI);

#else // No USB available, fallback to Serial
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

// --

void setupRxBenchmark()
{
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleControlChange(handleControlChange);
    MIDI.setHandleProgramChange(handleProgramChange);
    MIDI.setHandleAfterTouchChannel(handleAfterTouchChannel);
    MIDI.setHandleAfterTouchPoly(handleAfterTouchPoly);
    MIDI.setHandlePitchBend(handlePitchBend);
    MIDI.setHandleStart(handleStart);
    MIDI.setHandleStop(handleStop);
    MIDI.setHandleContinue(handleContinue);
    MIDI.setHandleClock(handleClock);
    MIDI.setHandleSystemExclusive(handleSysEx);
}

void startTxBenchmark()
{
    for (int i = 0; i < 128; ++i)
    {
        MIDI.sendNoteOn(i, 127, 1);
        MIDI.sendNoteOff(i, 127, 1);
    }
    for (int i = 0; i < 128; ++i)
    {
        MIDI.sendControlChange(i, 127, 1);
        MIDI.sendControlChange(i, 0, 1);
    }
    for (int i = 0; i < 128; ++i)
    {
        MIDI.sendProgramChange(i, 1);
    }
    MIDI.sendRealTime(midi::Start);
    MIDI.sendRealTime(midi::Stop);
    MIDI.sendRealTime(midi::Continue);
    MIDI.sendRealTime(midi::Clock);

    const byte length = 128;
    const byte data[128] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    };
    MIDI.sendSysEx(length, data, false);
}

// --

void printChannel(byte inChannel)
{
    Serial.print("[");
    Serial.print(inChannel);
    Serial.print("] ");
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
    printChannel(inChannel);
    Serial.print("Note Off\t");
    Serial.print(inNote);
    Serial.print(' ');
    Serial.println(inVelocity);
}
void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    printChannel(inChannel);
    Serial.print("Note On\t");
    Serial.print(inNote);
    Serial.print(' ');
    Serial.println(inVelocity);
}
void handleControlChange(byte inChannel, byte inControl, byte inValue)
{
    printChannel(inChannel);
    Serial.print("Control Change\t");
    Serial.print(inControl);
    Serial.print(' ');
    Serial.println(inValue);
}
void handleProgramChange(byte inChannel, byte inProgram)
{
    printChannel(inChannel);
    Serial.print("Program Change\t");
    Serial.println(inProgram);
}
void handleAfterTouchChannel(byte inChannel, byte inPressure)
{
    printChannel(inChannel);
    Serial.print("AT Channel\t");
    Serial.println(inPressure);
}
void handleAfterTouchPoly(byte inChannel, byte inNote, byte inPressure)
{
    printChannel(inChannel);
    Serial.print("AT Poly\t");
    Serial.print(inNote);
    Serial.print(' ');
    Serial.println(inPressure);
}
void handlePitchBend(byte inChannel, int inBend)
{
    printChannel(inChannel);
    Serial.print("Pitch Bend\t");
    Serial.println(inBend);
}
void handleStart()
{
    Serial.println("Start");
}
void handleStop()
{
    Serial.println("Stop");
}
void handleContinue()
{
    Serial.println("Continue");
}
void handleClock()
{
    Serial.println("Clock");
}
void handleSysEx(byte* inArray, unsigned inSize)
{
    Serial.print("SysEx\t[ ");
    for (unsigned i = 0; i < inSize; ++i)
    {
        Serial.print(inArray[i], HEX);
        Serial.print(' ');
    }
    Serial.println(']');
}

// --

void setup()
{
    Serial.begin(115200);
    while(!Serial); // Wait for the Serial Monitor to open

    MIDI.begin();
    MIDI.turnThruOff();

    setupRxBenchmark();
    startTxBenchmark();
}

void loop()
{
    MIDI.read();
}
