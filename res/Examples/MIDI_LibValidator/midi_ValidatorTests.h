#pragma once

// -----------------------------------------------------------------------------

#define NUM_TESTS   18

// -----------------------------------------------------------------------------

#define DECLARE_MIDI_TEST(Name)                                                 \
    template<class SerialClass>                                                 \
    bool Name(MIDI_CLASS(SerialClass)& inMidi)

#define IMPLEMENT_MIDI_TEST(Name, Instance)                                     \
    template<class SerialClass>                                                 \
    bool Name(MIDI_CLASS(SerialClass)& Instance)

// -----------------------------------------------------------------------------
    
static const char * testNames[] = 
{
  // 0123456789ABCDEF
    "NoteOn         ",
    "NoteOff        ",
    "ControlChange  ",
    "ProgramChange  ",
    "AftertouchMono ",
    "AftertouchPoly ",
    "PitchBend      ",
    "SysEx          ",
    "Clock          ",
    "Start          ",
    "Stop           ",
    "Continue       ",
    "ActiveSensing  ",
    "TimeCode       ",
    "SongSelect     ",
    "SongPosition   ",
    "TuneRequest    ",
    "SystemReset    ",
};

// -----------------------------------------------------------------------------

DECLARE_MIDI_TEST(testNoteOn);
DECLARE_MIDI_TEST(testNoteOff);
DECLARE_MIDI_TEST(testControlChange);
DECLARE_MIDI_TEST(testProgramChange);
DECLARE_MIDI_TEST(testAftertouchMono);
DECLARE_MIDI_TEST(testAftertouchPoly);
DECLARE_MIDI_TEST(testPitchBend);
DECLARE_MIDI_TEST(testSysEx);
DECLARE_MIDI_TEST(testClock);
DECLARE_MIDI_TEST(testStart);
DECLARE_MIDI_TEST(testStop);
DECLARE_MIDI_TEST(testContinue);
DECLARE_MIDI_TEST(testActiveSensing);
DECLARE_MIDI_TEST(testTimeCode);
DECLARE_MIDI_TEST(testSongSelect);
DECLARE_MIDI_TEST(testSongPosition);
DECLARE_MIDI_TEST(testTuneRequest);
DECLARE_MIDI_TEST(testSystemReset);

