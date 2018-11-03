#include "unit-tests.h"
#include <src/midi_UsbDefs.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

TEST(MidiUsbDefs, codeIndexNumberFromStatus)
{
    typedef midi::CodeIndexNumbers CIN;
    EXPECT_EQ(CIN::fromStatus(midi::InvalidType),           CIN::reserved);
    EXPECT_EQ(CIN::fromStatus(midi::NoteOff),               CIN::noteOff);
    EXPECT_EQ(CIN::fromStatus(midi::NoteOn),                CIN::noteOn);
    EXPECT_EQ(CIN::fromStatus(midi::AfterTouchPoly),        CIN::polyPressure);
    EXPECT_EQ(CIN::fromStatus(midi::ControlChange),         CIN::controlChange);
    EXPECT_EQ(CIN::fromStatus(midi::ProgramChange),         CIN::programChange);
    EXPECT_EQ(CIN::fromStatus(midi::AfterTouchChannel),     CIN::channelPressure);
    EXPECT_EQ(CIN::fromStatus(midi::PitchBend),             CIN::pitchBend);
    EXPECT_EQ(CIN::fromStatus(midi::Clock),                 CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::Start),                 CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::Continue),              CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::Stop),                  CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::ActiveSensing),         CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::SystemReset),           CIN::singleByte);
    EXPECT_EQ(CIN::fromStatus(midi::SystemExclusive),       CIN::sysExStart);
    EXPECT_EQ(CIN::fromStatus(midi::TuneRequest),           CIN::systemCommon1Byte);
    EXPECT_EQ(CIN::fromStatus(midi::TimeCodeQuarterFrame),  CIN::systemCommon2Bytes);
    EXPECT_EQ(CIN::fromStatus(midi::SongSelect),            CIN::systemCommon2Bytes);
    EXPECT_EQ(CIN::fromStatus(midi::SongPosition),          CIN::systemCommon3Bytes);
}

TEST(MidiUsbDefs, codeIndexNumberSizes)
{
    typedef midi::CodeIndexNumbers CIN;
    EXPECT_EQ(CIN::getSize(CIN::reserved),              0);
    EXPECT_EQ(CIN::getSize(CIN::misc),                  0);
    EXPECT_EQ(CIN::getSize(CIN::cableEvent),            0);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon2Bytes),    2);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon3Bytes),    3);
    EXPECT_EQ(CIN::getSize(CIN::sysExStart),            3);
    EXPECT_EQ(CIN::getSize(CIN::sysExContinue),         3);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon1Byte),     1);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds1Byte),        1);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds2Bytes),       2);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds3Bytes),       3);
    EXPECT_EQ(CIN::getSize(CIN::noteOff),               3);
    EXPECT_EQ(CIN::getSize(CIN::noteOn),                3);
    EXPECT_EQ(CIN::getSize(CIN::polyPressure),          3);
    EXPECT_EQ(CIN::getSize(CIN::controlChange),         3);
    EXPECT_EQ(CIN::getSize(CIN::programChange),         2);
    EXPECT_EQ(CIN::getSize(CIN::channelPressure),       2);
    EXPECT_EQ(CIN::getSize(CIN::pitchBend),             3);
    EXPECT_EQ(CIN::getSize(CIN::singleByte),            1);
}

END_UNNAMED_NAMESPACE
