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

void attachCallbacks()
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
    MIDI.begin(MIDI_CHANNEL_OMNI);

    // attachCallbacks();
    // Serial.begin(115200);
    // while(!Serial); // Wait for the Serial Monitor to open
}

void loop()
{
    MIDI.read();
}
