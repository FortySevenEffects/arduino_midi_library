#include "unit-tests.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

template<bool RunningStatus, bool OneByteParsing>
struct VariableSettings : public midi::DefaultSettings
{
    static const bool UseRunningStatus = RunningStatus;
    static const bool Use1ByteParsing  = OneByteParsing;
};

template<bool A, bool B>
const bool VariableSettings<A, B>::UseRunningStatus;
template<bool A, bool B>
const bool VariableSettings<A, B>::Use1ByteParsing;

using namespace testing;
typedef test_mocks::SerialMock<32> SerialMock;
typedef midi::MidiInterface<SerialMock> MidiInterface;

// --

TEST(MidiOutput, sendSingle)
{
    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[3] = { 0 };

    midi.begin();
    midi.send(midi::NoteOn, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ElementsAre(0x9b, 47, 42));
}

TEST(MidiOutput, sendWithRunningStatus)
{
    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[5] = { 0 };

    midi.begin();
    EXPECT_EQ(MidiInterface::Settings::UseRunningStatus, true);
    EXPECT_EQ(serial.mTxBuffer.isEmpty(), true);
    midi.send(midi::NoteOn, 47, 42, 12);
    midi.send(midi::NoteOn, 42, 47, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);
    serial.mTxBuffer.read(buffer, 5);
    EXPECT_THAT(buffer, ElementsAre(0x9b, 47, 42, 42, 47));
}

TEST(MidiOutput, sendWithoutRunningStatus)
{
    typedef VariableSettings<false, true> Settings; // No running status
    typedef midi::MidiInterface<SerialMock, Settings> MidiInterface;

    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[6] = { 0 };

    // Same status byte
    midi.begin();
    EXPECT_EQ(MidiInterface::Settings::UseRunningStatus, false);
    EXPECT_EQ(serial.mTxBuffer.isEmpty(), true);
    midi.send(midi::NoteOn, 47, 42, 12);
    midi.send(midi::NoteOn, 42, 47, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(buffer, 6);
    EXPECT_THAT(buffer, ElementsAre(0x9b, 47, 42, 0x9b, 42, 47));

    // Different status byte
    midi.begin();
    midi.send(midi::NoteOn,  47, 42, 12);
    midi.send(midi::NoteOff, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(buffer, 6);
    EXPECT_THAT(buffer, ElementsAre(0x9b, 47, 42, 0x8b, 47, 42));
}

TEST(MidiOutput, sendBreakingRunningStatus)
{
    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[6] = { 0 };

    midi.begin();
    midi.send(midi::NoteOn,  47, 42, 12);
    midi.send(midi::NoteOff, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(buffer, 6);
    EXPECT_THAT(buffer, ElementsAre(0x9b, 47, 42, 0x8b, 47, 42));
}

TEST(MidiOutput, sendMultipleNoteOff)
{
    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[5] = { 0 };

    midi.begin();
    midi.send(midi::NoteOff, 10, 11, 12);
    midi.send(midi::NoteOff, 12, 13, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);
    serial.mTxBuffer.read(buffer, 5);
    EXPECT_THAT(buffer, ElementsAre(0x8b, 10, 11, 12, 13));
}

/* TEST(MidiOutput, Issue41)
{
    typedef VariableSettings<true, false> Settings; // Running status, multibyte parsing
    typedef midi::MidiInterface<SerialMock, Settings> MidiInterface;

    // #41: issue with sending series of ControlChange
    SerialMock serial;
    MidiInterface midi(serial);
    test_mocks::uint8 buffer[9] = { 0 };

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.turnThruOff();

    // Simulate some pitch bend messages
    serial.mRxBuffer.write(0xe0);
    serial.mRxBuffer.write(0);
    serial.mRxBuffer.write(12);
    serial.mRxBuffer.write(0);
    serial.mRxBuffer.write(42);
    serial.mRxBuffer.write(0);
    serial.mRxBuffer.write(47);
    midi.read();

    EXPECT_EQ(serial.mTxBuffer.getLength(), 9);
    serial.mTxBuffer.read(buffer, 9);
    EXPECT_THAT(buffer, ElementsAre(0xbb, 80, 42, 80, 47, 80, 42, 80, 47));
} */

// --

END_UNNAMED_NAMESPACE
