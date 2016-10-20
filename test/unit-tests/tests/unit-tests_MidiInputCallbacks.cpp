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
typedef midi::MidiInterface<SerialMock> MidiInterface;

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

MidiInterface* midi;

// --

void handleNoteOn(byte inChannel, byte inPitch, byte inVelocity)
{
    EXPECT_NE(midi, nullptr);
    midi->sendNoteOn(inPitch, inVelocity, inChannel);
}

TEST(MidiInputCallbacks, noteOn)
{
    EXPECT_EQ(midi, nullptr);
    SerialMock serial;
    midi = new MidiInterface(serial);


    midi->setHandleNoteOn(handleNoteOn);
    midi->begin(MIDI_CHANNEL_OMNI);
    midi->turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x9b, 12, 34 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi->read(), false);
    EXPECT_EQ(midi->read(), false);
    EXPECT_EQ(midi->read(), true);
    EXPECT_EQ(midi->getType(),       midi::NoteOn);
    EXPECT_EQ(midi->getChannel(),    12);
    EXPECT_EQ(midi->getData1(),      12);
    EXPECT_EQ(midi->getData2(),      34);

    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);


    delete midi;
    midi = nullptr;
}

END_UNNAMED_NAMESPACE
