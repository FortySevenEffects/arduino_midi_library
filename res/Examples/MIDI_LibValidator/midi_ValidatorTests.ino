
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
                    inMidi.getData2() == 42 &&
                    inMidi.getChannel() == 3;
    return result;
}
