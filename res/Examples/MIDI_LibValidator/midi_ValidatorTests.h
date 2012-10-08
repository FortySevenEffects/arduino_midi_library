#pragma once

// -----------------------------------------------------------------------------

#define NUM_TESTS 4

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
    "NoteOn        ",
    "NoteOff       ",
    "ControlChange ",
    "ProgramChange ",
    
};

// -----------------------------------------------------------------------------

DECLARE_MIDI_TEST(testNoteOn);
DECLARE_MIDI_TEST(testNoteOff);
DECLARE_MIDI_TEST(testControlChange);
DECLARE_MIDI_TEST(testProgramChange);
