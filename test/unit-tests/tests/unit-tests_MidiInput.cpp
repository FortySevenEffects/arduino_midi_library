#include "unit-tests.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

typedef test_mocks::SerialMock<32> SerialMock;
typedef midi::MidiInterface<SerialMock> MidiInterface;

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
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xf9), midi::InvalidType);
    EXPECT_EQ(MidiInterface::getTypeFromStatusByte(0xfd), midi::InvalidType);
}

TEST(MidiInput, begin)
{
    SerialMock serial;
    MidiInterface midi(serial);

    // Default channel
    midi.begin();
    EXPECT_EQ(serial.mBaudrate, 31250);
    EXPECT_EQ(midi.getInputChannel(), 1);

    // Specific channel
    midi.begin(12);
    EXPECT_EQ(serial.mBaudrate, 31250);
    EXPECT_EQ(midi.getInputChannel(), 12);
}

END_UNNAMED_NAMESPACE
