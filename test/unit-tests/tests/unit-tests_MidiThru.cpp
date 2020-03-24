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
typedef midi::SerialMIDI<SerialMock> Transport;
typedef midi::MidiInterface<Transport> MidiInterface;
typedef std::vector<byte> Buffer;

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

// -----------------------------------------------------------------------------

TEST(MidiThru, defaultValues)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.begin(); // Should not change the state
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
}

TEST(MidiThru, beginEnablesThru)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);
    midi.begin();
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
}

TEST(MidiThru, setGet)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);

    midi.turnThruOn();
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.turnThruOn(midi::Thru::SameChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::SameChannel);
    midi.turnThruOn(midi::Thru::DifferentChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::DifferentChannel);

    midi.setThruFilterMode(midi::Thru::Full);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.setThruFilterMode(midi::Thru::SameChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::SameChannel);
    midi.setThruFilterMode(midi::Thru::DifferentChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::DifferentChannel);
    midi.setThruFilterMode(midi::Thru::Off);
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);
}

TEST(MidiThru, off)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.turnThruOff();

    static const unsigned rxSize = 5;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
}

TEST(MidiThru, full)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::Full);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9b, 12, 34
    }));

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9c, 56, 78
    }));
}

TEST(MidiThru, sameChannel)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    midi.begin(12);
    midi.setThruFilterMode(midi::Thru::SameChannel);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9b, 12, 34
    }));
}

TEST(MidiThru, sameChannelOmni) // Acts like full
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::SameChannel);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9b, 12, 34
    }));

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(serial.mTxBuffer.getLength(), 3); // Not using TX running status
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9c, 56, 78
    }));
}

TEST(MidiThru, differentChannel)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    midi.begin(12);
    midi.setThruFilterMode(midi::Thru::DifferentChannel);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9c, 56, 78
    }));
}

TEST(MidiThru, differentChannelOmni) // Acts like off
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::DifferentChannel);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
}

TEST(MidiThru, multiByteThru)
{
    typedef VariableSettings<false, false> MultiByteParsing;
    typedef midi::MidiInterface<Transport, MultiByteParsing> MultiByteMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    MultiByteMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::Full);

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);

    buffer.clear();
    buffer.resize(6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9b, 12, 34, 0x9b, 56, 78
    }));
}

TEST(MidiThru, withTxRunningStatus)
{
    typedef VariableSettings<true, true> Settings;
    typedef midi::MidiInterface<Transport, Settings> RsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::Full);

    static const unsigned rxSize = 5;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    buffer.clear();
    buffer.resize(3);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x9b, 12, 34
    }));

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    EXPECT_EQ(midi.read(), true);

    buffer.clear();
    buffer.resize(2);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 2);
    serial.mTxBuffer.read(&buffer[0], 2);
    EXPECT_THAT(buffer, ElementsAreArray({
        56, 78
    }));
}

TEST(MidiThru, invalidMode)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilterMode(midi::Thru::Mode(42));

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = { 0x9b, 12, 34, 0x9c, 56, 78 };
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
}

END_UNNAMED_NAMESPACE
