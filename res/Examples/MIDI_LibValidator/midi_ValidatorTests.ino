#include <Arduino.h>
#include "midi_ValidatorTests.h"

// -----------------------------------------------------------------------------

IMPLEMENT_MIDI_TEST(testNoteOn, inMidi)
{
    inMidi.sendNoteOn(12, 42, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::NoteOn &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 42 &&
                    inMidi.getChannel() == 3;
    return result;
}

IMPLEMENT_MIDI_TEST(testNoteOff, inMidi)
{
    inMidi.sendNoteOff(12, 42, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::NoteOff &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 42 &&
                    inMidi.getChannel() == 3;
    return result;
}

IMPLEMENT_MIDI_TEST(testControlChange, inMidi)
{
    inMidi.sendControlChange(12, 42, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::ControlChange &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 42 &&
                    inMidi.getChannel() == 3;
    return result;
}

IMPLEMENT_MIDI_TEST(testProgramChange, inMidi)
{
    inMidi.sendProgramChange(12, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::ProgramChange &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 0 &&
                    inMidi.getChannel() == 3;
    return result;
}

IMPLEMENT_MIDI_TEST(testAftertouchMono, inMidi)
{
    inMidi.sendAfterTouch(12, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::AfterTouchChannel &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 0 &&
                    inMidi.getChannel() == 3;
    return result;
}

IMPLEMENT_MIDI_TEST(testAftertouchPoly, inMidi)
{
    inMidi.sendPolyPressure(12, 42, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::AfterTouchPoly &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 42 &&
                    inMidi.getChannel() == 3;
    return result;
}

// -----------------------------------------------------------------------------

volatile int pitchBendMemory = 0;

void pitchBendCallback(byte inChannel, int inValue)
{
    pitchBendMemory = inValue;
}

IMPLEMENT_MIDI_TEST(testPitchBend, inMidi)
{
    inMidi.setHandlePitchBend(pitchBendCallback);
    pitchBendMemory = 0;
    inMidi.sendPitchBend((int)1234, 3);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::PitchBend &&
                    pitchBendMemory == 1234 &&
                    inMidi.getChannel() == 3;
    return result;
}

// -----------------------------------------------------------------------------

IMPLEMENT_MIDI_TEST(testSysEx, inMidi)
{
    static const byte testData[13] =
    {
        'H', 'e', 'l', 'l', 'o', ',', ' ',
        'w', 'o', 'r', 'l', 'd', 0
    };

    inMidi.sendSysEx(13, testData);

    while (inMidi.read() == false) { }

    bool result =   inMidi.getSysExArrayLength() == 15 &&       // 13 + F0 + F7
                    memcmp((const char*)inMidi.getSysExArray()+1,
                           (const char*)testData,
                           13) == 0;
    return result;
}

IMPLEMENT_MIDI_TEST(testClock, inMidi)
{
    inMidi.sendRealTime(midi::Clock);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::Clock;
}

IMPLEMENT_MIDI_TEST(testStart, inMidi)
{
    inMidi.sendRealTime(midi::Start);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::Start;
}

IMPLEMENT_MIDI_TEST(testStop, inMidi)
{
    inMidi.sendRealTime(midi::Stop);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::Stop;
}

IMPLEMENT_MIDI_TEST(testContinue, inMidi)
{
    inMidi.sendRealTime(midi::Continue);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::Continue;
}

IMPLEMENT_MIDI_TEST(testActiveSensing, inMidi)
{
    inMidi.sendRealTime(midi::ActiveSensing);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::ActiveSensing;
}

// -----------------------------------------------------------------------------

// \todo Add callback to process parsed value

volatile byte timeCodeMemory = 0;

void timeCodeCallback(byte inData)
{
    timeCodeMemory = inData;
}

IMPLEMENT_MIDI_TEST(testTimeCode, inMidi)
{
    timeCodeMemory = 0;
    inMidi.setHandleTimeCodeQuarterFrame(timeCodeCallback);
    inMidi.sendTimeCodeQuarterFrame(0x07, 0x0F);
    while (inMidi.read() == false) { }
    bool result =   inMidi.getType()  == midi::TimeCodeQuarterFrame &&
                    timeCodeMemory == 0x7F &&
                    inMidi.getChannel() == 0;
    return result;
}

// -----------------------------------------------------------------------------

IMPLEMENT_MIDI_TEST(testSongSelect, inMidi)
{
    inMidi.sendSongSelect(12);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::SongSelect &&
                    inMidi.getData1() == 12 &&
                    inMidi.getData2() == 0 &&
                    inMidi.getChannel() == 0;
    return result;
}

// -----------------------------------------------------------------------------

volatile int songPositionMemory = 0;

void songPositionCallback(unsigned int inPosition)
{
    songPositionMemory = inPosition;
}

IMPLEMENT_MIDI_TEST(testSongPosition, inMidi)
{
    songPositionMemory = 0;
    inMidi.setHandleSongPosition(songPositionCallback);
    inMidi.sendSongPosition(12345);
    while (inMidi.read() == false) { }

    bool result =   inMidi.getType()  == midi::SongPosition &&
                    songPositionMemory == 12345 &&
                    inMidi.getChannel() == 0;
    return result;
}

// -----------------------------------------------------------------------------

IMPLEMENT_MIDI_TEST(testTuneRequest, inMidi)
{
    inMidi.sendTuneRequest();
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::TuneRequest;
}

IMPLEMENT_MIDI_TEST(testSystemReset, inMidi)
{
    inMidi.sendRealTime(midi::SystemReset);
    while (inMidi.read() == false) { }
    return inMidi.getType() == midi::SystemReset;
}
