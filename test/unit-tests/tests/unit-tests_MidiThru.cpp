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
typedef midi::Message<midi::DefaultSettings::SysExMaxSize> MidiMessage;

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

SerialMock serial;
Transport transport(serial);
MidiInterface midi((Transport&)transport);

bool thruFilterSameChannel(const MidiMessage& inMessage)
{
    if (!midi.isChannelMessage(inMessage.type))
        return true;

    return MIDI_CHANNEL_OMNI == midi.getInputChannel() ||
           inMessage.channel == midi.getInputChannel();
}

bool thruFilterDifferentChannel(const MidiMessage& inMessage)
{
    if (!midi.isChannelMessage(inMessage.type))
        return true;

    return MIDI_CHANNEL_OMNI != midi.getInputChannel() &&
           inMessage.channel != midi.getInputChannel();
}

MidiMessage thruMapNoteOnFullVelocity(const MidiMessage& inMessage)
{
    if (inMessage.type != midi::MidiType::NoteOn)
        return inMessage;

    MidiMessage modified = inMessage;
    modified.data2 = 127;
    return modified;
}

// -----------------------------------------------------------------------------

TEST(MidiThru, defaultValues)
{
    EXPECT_EQ(midi.getThruState(),  true);
    midi.begin(); // Should not change the state
    EXPECT_EQ(midi.getThruState(),  true);
}

TEST(MidiThru, beginEnablesThru)
{
    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);
    midi.begin();
    EXPECT_EQ(midi.getThruState(),  true);
}

TEST(MidiThru, setGet)
{
    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);

    midi.turnThruOn();
    EXPECT_EQ(midi.getThruState(),  true);
}

TEST(MidiThru, off)
{
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
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);

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
    Buffer buffer;

    midi.begin(12);
    midi.setThruFilter(thruFilterSameChannel);

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
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilter(thruFilterSameChannel);

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
    Buffer buffer;

    midi.begin(12);
    midi.setThruFilter(thruFilterDifferentChannel);

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
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruFilter(thruFilterDifferentChannel);

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

    MultiByteMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);

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

    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);

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

TEST(MidiThru, mapNoteOnFullVelocity)
{
    Buffer buffer;

    midi.begin(MIDI_CHANNEL_OMNI);
    midi.setThruMap(thruMapNoteOnFullVelocity);

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
        0x9b, 12, 127                                  // thru message full velocity
    }));
    EXPECT_EQ(midi.getType(),       midi::NoteOn);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);               // mMessage velocity unchanged

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
        0x9c, 56, 127                                  // thru message full velocity
    }));
    EXPECT_EQ(midi.getType(),       midi::NoteOn);
    EXPECT_EQ(midi.getChannel(),    13);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);               // mMessage velocity unchanged
}

END_UNNAMED_NAMESPACE
