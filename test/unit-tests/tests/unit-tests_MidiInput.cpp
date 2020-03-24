#include "unit-tests.h"
#include "unit-tests_Settings.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
USING_NAMESPACE_UNIT_TESTS

typedef test_mocks::SerialMock<32> SerialMock;
typedef midi::SerialMIDI<SerialMock> serialMIDI;
typedef midi::MidiInterface<serialMIDI> MidiInterface;

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

TEST(MidiInput, getTypeFromStatusByte)
{
    // Channel Messages
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0x81), midi::NoteOff);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0x92), midi::NoteOn);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xa3), midi::AfterTouchPoly);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xb4), midi::ControlChange);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xc5), midi::ProgramChange);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xd6), midi::AfterTouchChannel);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xe7), midi::PitchBend);

    // System Messages
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf0), midi::SystemExclusive);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf1), midi::TimeCodeQuarterFrame);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf2), midi::SongPosition);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf3), midi::SongSelect);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf6), midi::TuneRequest);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf8), midi::Clock);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfa), midi::Start);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfb), midi::Continue);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfc), midi::Stop);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfe), midi::ActiveSensing);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xff), midi::SystemReset);

    // Invalid Messages
    for (int i = 0; i < 0x80; ++i)
    {
        EXPECT_EQ(MidiInterface::getTypeFromStatusByte(i), midi::InvalidType);
    }
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf4), midi::InvalidType);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf5), midi::InvalidType);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfd), midi::InvalidType);
}

TEST(MidiInput, getChannelFromStatusByte)
{
    EXPECT_EQ(MidiInterface::getChannelFromStatusByte(0x00), 1);
    EXPECT_EQ(MidiInterface::getChannelFromStatusByte(0x80), 1);
    EXPECT_EQ(MidiInterface::getChannelFromStatusByte(0x94), 5);
    EXPECT_EQ(MidiInterface::getChannelFromStatusByte(0xaf), 16);
}

TEST(MidiInput, isChannelMessage)
{
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::InvalidType),           false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::NoteOff),               true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::NoteOn),                true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::AfterTouchPoly),        true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::ControlChange),         true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::ProgramChange),         true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::AfterTouchChannel),     true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::PitchBend),             true);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::SystemExclusive),       false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::TimeCodeQuarterFrame),  false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::SongPosition),          false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::SongSelect),            false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::TuneRequest),           false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::Clock),                 false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::Start),                 false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::Continue),              false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::Stop),                  false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::ActiveSensing),         false);
    EXPECT_EQ(MidiInterface::isChannelMessage(midi::SystemReset),           false);
}

// --

TEST(MidiInput, begin)
{
    SerialMock serial;
    midi::SerialMIDI<SerialMock> transport(serial);
    midi::MidiInterface<midi::SerialMIDI<Type>> midi((midi::SerialMIDI<SerialMock>&)transport);

    // Default channel
    midi.begin();
 //   EXPECT_EQ(serial.mBaudrate, 31250);
    EXPECT_EQ(midi.getInputChannel(), 1);

    // Specific channel
    midi.begin(12);
 //   EXPECT_EQ(serial.mBaudrate, 31250);
    EXPECT_EQ(midi.getInputChannel(), 12);
}

END_UNNAMED_NAMESPACE
