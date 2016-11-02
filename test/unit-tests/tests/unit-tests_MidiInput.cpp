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

TEST(MidiInput, initInputChannel)
{
    SerialMock serial;
    MidiInterface midi(serial);

    EXPECT_EQ(midi.getInputChannel(), 0);
    midi.setInputChannel(12);
    EXPECT_EQ(midi.getInputChannel(), 12);
}

TEST(MidiInput, initMessage)
{
    SerialMock serial;
    MidiInterface midi(serial);
    EXPECT_EQ(midi.getType(),               midi::InvalidType);
    EXPECT_EQ(midi.getChannel(),            0);
    EXPECT_EQ(midi.getData1(),              0);
    EXPECT_EQ(midi.getData2(),              0);
    EXPECT_EQ(midi.getSysExArrayLength(),   unsigned(0));
    EXPECT_EQ(midi.check(),                 false);
}

TEST(MidiInput, channelFiltering)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x9b, 12, 34 };
    midi.begin(4); // Mistmatching channel
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
}

TEST(MidiInput, noRxData)
{
    SerialMock serial;
    MidiInterface midi(serial);
    midi.begin();
    EXPECT_EQ(midi.read(), false);
}

TEST(MidiInput, inputDisabled)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x9b, 12, 34 };
    midi.begin(MIDI_CHANNEL_OFF); // Invalid channel
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
}

TEST(MidiInput, multiByteParsing)
{
    typedef VariableSettings<false, false> Settings;
    typedef midi::MidiInterface<SerialMock, Settings> MultiByteMidiInterface;

    SerialMock serial;
    MultiByteMidiInterface midi(serial);
    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x9b, 12, 34 };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), true);
}

TEST(MidiInput, noteOn)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 10;
    static const byte rxData[rxSize] = {
        0x9b, 12, 34,
        0x9b, 56, 78,
              12, 34,   // Running status
              56, 0     // NoteOn with null velocity interpreted as NoteOff
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    // First NoteOn
    EXPECT_EQ(midi.getType(),       midi::NoteOn);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::NoteOn);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::NoteOn);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::NoteOff);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, noteOff)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 8;
    static const byte rxData[rxSize] = {
        0x8b, 12, 34,
        0x8b, 56, 78,
              12, 34,   // Running status
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    // First NoteOn
    EXPECT_EQ(midi.getType(),       midi::NoteOff);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::NoteOff);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::NoteOff);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);
}

TEST(MidiInput, programChange)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = {
        0xc3, 12, 34,
        0xc4, 56, 78
    };
    midi.begin(MIDI_CHANNEL_OMNI);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ProgramChange);
    EXPECT_EQ(midi.getChannel(),    4);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ProgramChange);
    EXPECT_EQ(midi.getChannel(),    4);
    EXPECT_EQ(midi.getData1(),      34);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ProgramChange);
    EXPECT_EQ(midi.getChannel(),    5);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ProgramChange);
    EXPECT_EQ(midi.getChannel(),    5);
    EXPECT_EQ(midi.getData1(),      78);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, controlChange)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 8;
    static const byte rxData[rxSize] = {
        0xbb, 12, 34,
        0xbb, 56, 78,
              12, 34
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    // First NoteOn
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);
}

TEST(MidiInput, pitchBend)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 8;
    static const byte rxData[rxSize] = {
        0xeb, 12, 34,
        0xeb, 56, 78,
              12, 34
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    // First NoteOn
    EXPECT_EQ(midi.getType(),       midi::PitchBend);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::PitchBend);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::PitchBend);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);
}

TEST(MidiInput, afterTouchPoly)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 8;
    static const byte rxData[rxSize] = {
        0xab, 12, 34,
        0xab, 56, 78,
              12, 34
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    // First NoteOn
    EXPECT_EQ(midi.getType(),       midi::AfterTouchPoly);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchPoly);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchPoly);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);
}

TEST(MidiInput, afterTouchChannel)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = {
        0xd3, 12, 34,
        0xd4, 56, 78
    };
    midi.begin(MIDI_CHANNEL_OMNI);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchChannel);
    EXPECT_EQ(midi.getChannel(),    4);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchChannel);
    EXPECT_EQ(midi.getChannel(),    4);
    EXPECT_EQ(midi.getData1(),      34);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchChannel);
    EXPECT_EQ(midi.getChannel(),    5);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::AfterTouchChannel);
    EXPECT_EQ(midi.getChannel(),    5);
    EXPECT_EQ(midi.getData1(),      78);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, sysExWithinBufferSize)
{
    typedef VariableSysExSettings<1024> Settings;
    typedef test_mocks::SerialMock<2048> LargerSerialMock;
    typedef midi::MidiInterface<LargerSerialMock, Settings> LargerMidiInterface;

    LargerSerialMock serial;
    LargerMidiInterface midi(serial);

    // Short Frame < 256
    {
        static const unsigned frameLength = 15;
        static const byte frame[frameLength] = {
            0xf0, 'H','e','l','l','o',',',' ','W','o','r','l','d','!', 0xf7
        };

        midi.begin();
        serial.mRxBuffer.write(frame, frameLength);
        for (unsigned i = 0; i < frameLength - 1; ++i)
        {
            EXPECT_EQ(midi.read(), false);
        }
        EXPECT_EQ(midi.read(), true); // 0xf7

        EXPECT_EQ(midi.getSysExArrayLength(), frameLength);
        const std::vector<byte> sysExData(midi.getSysExArray(),
                                          midi.getSysExArray() + frameLength);
        EXPECT_THAT(sysExData, ElementsAreArray(frame));
    }
    // Long Frame
    {
        static const unsigned frameLength = 957;
        static const byte frame[frameLength] = {
            0xf0,
            'L','o','r','e','m',' ','i','p','s','u','m',' ','d','o','l','o','r',' ','s','i','t',' ','a','m','e','t',',',' ',
            'c','o','n','s','e','c','t','e','t','u','r',' ','a','d','i','p','i','s','c','i','n','g',' ','e','l','i','t','.',' ','P','r','o','i','n',' ','m','a','x','i','m','u','s',' ','d','u','i',' ','a',' ','m','a','s','s','a',' ','m','a','x','i','m','u','s',',',' ',
            'a',' ','v','e','s','t','i','b','u','l','u','m',' ','m','i',' ','v','e','n','e','n','a','t','i','s','.',' ','C','r','a','s',' ','s','i','t',' ','a','m','e','t',' ','e','x',' ','i','d',' ','v','e','l','i','t',' ','s','u','s','c','i','p','i','t',' ','p','h','a','r','e','t','r','a',' ','e','g','e','t',            ' ','a',' ','t','u','r','p','i','s','.',' ','P','h','a','s','e','l','l','u','s',' ','i','n','t','e','r','d','u','m',' ','m','e','t','u','s',' ','a','c',' ','s','a','g','i','t','t','i','s',' ','c','u','r','s','u','s','.',' ','N','a','m',' ','q','u','i','s',' ','e','s','t',' ','a','t',' ','n','i','s',            'l',' ','u','l','l','a','m','c','o','r','p','e','r',' ','e','g','e','s','t','a','s',' ','p','u','l','v','i','n','a','r',' ','e','u',' ','e','r','a','t','.',' ','D','u','i','s',' ','a',' ','e','l','i','t',' ','d','i','g','n','i','s','s','i','m',',',' ',
            'v','e','s','t','i','b','u','l','u','m',' ','e','r','o','s',' ','v','e','l',',',' ',
            't','e','m','p','u','s',' ','n','i','s','l','.',' ','A','e','n','e','a','n',' ','t','u','r','p','i','s',' ','n','u','n','c',',',' ',
            'c','u','r','s','u','s',' ','v','e','l',' ','l','a','c','i','n','i','a',' ','n','o','n',',',' ',
            'p','h','a','r','e','t','r','a',' ','e','g','e','t',' ','s','a','p','i','e','n','.',' ','D','u','i','s',' ','c','o','n','d','i','m','e','n','t','u','m',',',' ',
            'l','a','c','u','s',' ','a','t',' ','p','u','l','v','i','n','a','r',' ','t','e','m','p','o','r',',',' ',
            'l','e','o',' ','l','i','b','e','r','o',' ','v','o','l','u','t','p','a','t',' ','n','i','s','l',',',' ',
            'e','g','e','t',' ','p','o','r','t','t','i','t','o','r',' ','l','o','r','e','m',' ','m','i',' ','s','e','d',' ','m','a','g','n','a','.',' ','D','u','i','s',' ','d','i','c','t','u','m',',',' ',
            'm','a','s','s','a',' ','v','e','l',' ','e','u','i','s','m','o','d',' ','i','n','t','e','r','d','u','m',',',' ',
            'l','o','r','e','m',' ','m','i',' ','e','g','e','s','t','a','s',' ','e','l','i','t',',',' ',
            'h','e','n','d','r','e','r','i','t',' ','t','i','n','c','i','d','u','n','t',' ','e','s','t',' ','a','r','c','u',' ','a',' ','l','i','b','e','r','o','.',' ','I','n','t','e','r','d','u','m',' ','e','t',' ','m','a','l','e','s','u','a','d','a',' ','f','a','m','e','s',' ','a','c',' ','a','n','t','e',' ',            'i','p','s','u','m',' ','p','r','i','m','i','s',' ','i','n',' ','f','a','u','c','i','b','u','s','.',' ','C','u','r','a','b','i','t','u','r',' ','v','e','h','i','c','u','l','a',' ','m','a','g','n','a',' ','l','i','b','e','r','o',',',' ',
            'a','t',' ','r','h','o','n','c','u','s',' ','s','e','m',' ','o','r','n','a','r','e',' ','a','.',' ','I','n',' ','e','l','e','m','e','n','t','u','m',',',' ',
            'e','l','i','t',' ','e','t',' ','c','o','n','g','u','e',' ','p','u','l','v','i','n','a','r',',',' ',
            'm','a','s','s','a',' ','v','e','l','i','t',' ','c','o','m','m','o','d','o',' ','v','e','l','i','t',',',' ',
            'n','o','n',' ','e','l','e','m','e','n','t','u','m',' ','p','u','r','u','s',' ','l','i','g','u','l','a',' ','e','g','e','t',' ','l','a','c','u','s','.',' ','D','o','n','e','c',' ','e','f','f','i','c','i','t','u','r',' ','n','i','s','i',' ','e','u',' ','u','l','t','r','i','c','e','s',' ','e','f','f',            'i','c','i','t','u','r','.',' ','D','o','n','e','c',' ','n','e','q','u','e',' ','d','u','i',',',' ',
            'u','l','l','a','m','c','o','r','p','e','r',' ','i','d',' ','m','o','l','e','s','t','i','e',' ','q','u','i','s',',',' ',
            'c','o','n','s','e','q','u','a','t',' ','s','i','t',' ','a','m','e','t',' ','l','i','g','u','l','a','.',
            0xf7,
        };
        midi.begin();
        serial.mRxBuffer.write(frame, frameLength);
        for (unsigned i = 0; i < frameLength - 1; ++i)
        {
            EXPECT_EQ(midi.read(), false);
        }
        EXPECT_EQ(serial.mRxBuffer.getLength(), 1);
        EXPECT_EQ(serial.mRxBuffer.peek(), 0xf7);
        EXPECT_EQ(midi.read(), true);
    }
}

TEST(MidiInput, sysExOverBufferSize)
{
    typedef VariableSysExSettings<8> Settings;
    typedef midi::MidiInterface<SerialMock, Settings> SmallMidiInterface;

    SerialMock serial;
    SmallMidiInterface midi(serial);

    static const unsigned frameLength = 15;
    static const byte frame[frameLength] = {
        0xf0, 'H','e','l','l','o',',',' ','W','o','r','l','d','!', 0xf7
    };

    midi.begin();
    serial.mRxBuffer.write(frame, frameLength);

    for (unsigned i = 0; i < frameLength - 1; ++i)
    {
        EXPECT_EQ(midi.read(), false);
    }
    EXPECT_EQ(midi.read(), false);
}

TEST(MidiInput, mtcQuarterFrame)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 4;
    static const byte rxData[rxSize] = {
        0xf1, 12,
        0xf1, 42
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::TimeCodeQuarterFrame);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::TimeCodeQuarterFrame);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      42);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, songPosition)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = {
        0xf2, 12, 34,
        0xf2, 56, 78
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::SongPosition);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::SongPosition);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      56);
    EXPECT_EQ(midi.getData2(),      78);
}

TEST(MidiInput, songSelect)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 4;
    static const byte rxData[rxSize] = {
        0xf3, 12,
        0xf3, 42
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    // 1 byte parsing
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::SongSelect);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);

    EXPECT_EQ(midi.getType(),       midi::SongSelect);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      42);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, tuneRequest)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 1;
    static const byte rxData[rxSize] = {
        0xf6
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::TuneRequest);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, realTime)
{
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 8;
    static const byte rxData[rxSize] = {
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::Clock);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false); // 0xf9 = undefined

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::Start);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::Continue);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::Stop);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false); // 0xfd = undefined

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ActiveSensing);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::SystemReset);
    EXPECT_EQ(midi.getChannel(),    0);
    EXPECT_EQ(midi.getData1(),      0);
    EXPECT_EQ(midi.getData2(),      0);
}

// --

TEST(MidiInput, interleavedRealTime)
{
    SerialMock serial;
    MidiInterface midi(serial);

    // Interleaved Clocks between NoteOn / Off messages (with running status)
    {
        static const unsigned rxSize = 13;
        static const byte rxData[rxSize] = {
            0x9b, 12, 0xf8, 34,
            12, 0,
            42, 0xf8, 127,
            0xf8,
            42, 0xf8, 0
        };
        midi.begin(12);
        serial.mRxBuffer.write(rxData, rxSize);
        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), false);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::Clock);
        EXPECT_EQ(midi.getChannel(),    0);
        EXPECT_EQ(midi.getData1(),      0);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::NoteOn);
        EXPECT_EQ(midi.getChannel(),    12);
        EXPECT_EQ(midi.getData1(),      12);
        EXPECT_EQ(midi.getData2(),      34);

        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::NoteOff);
        EXPECT_EQ(midi.getChannel(),    12);
        EXPECT_EQ(midi.getData1(),      12);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::Clock);
        EXPECT_EQ(midi.getChannel(),    0);
        EXPECT_EQ(midi.getData1(),      0);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::NoteOn);
        EXPECT_EQ(midi.getChannel(),    12);
        EXPECT_EQ(midi.getData1(),      42);
        EXPECT_EQ(midi.getData2(),      127);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::Clock);
        EXPECT_EQ(midi.getChannel(),    0);
        EXPECT_EQ(midi.getData1(),      0);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::Clock);
        EXPECT_EQ(midi.getChannel(),    0);
        EXPECT_EQ(midi.getData1(),      0);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::NoteOff);
        EXPECT_EQ(midi.getChannel(),    12);
        EXPECT_EQ(midi.getData1(),      42);
        EXPECT_EQ(midi.getData2(),      0);
    }
    // Interleaved ActiveSensing between SysEx
    {
        static const unsigned rxSize = 6;
        static const byte rxData[rxSize] = {
            0xf0, 12, 34, 0xfe, 56, 0xf7
        };
        midi.begin(12);
        serial.mRxBuffer.write(rxData, rxSize);
        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), false);

        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getType(),       midi::ActiveSensing);
        EXPECT_EQ(midi.getChannel(),    0);
        EXPECT_EQ(midi.getData1(),      0);
        EXPECT_EQ(midi.getData2(),      0);

        EXPECT_EQ(midi.read(), false);
        EXPECT_EQ(midi.read(), true);
        EXPECT_EQ(midi.getSysExArrayLength(), rxSize - 1);
        const std::vector<byte> sysExData(midi.getSysExArray(),
                                          midi.getSysExArray() + rxSize - 1);
        EXPECT_THAT(sysExData, ElementsAreArray({
            0xf0, 12, 34, 56, 0xf7
        }));
    }
}

TEST(MidiInput, strayEox)
{
    // A stray End of Exclusive will reset the parser, but should it ?
    SerialMock serial;
    MidiInterface midi(serial);
    static const unsigned rxSize = 4;
    static const byte rxData[rxSize] = {
        0x8b, 42, 0xf7, 12
    };
    midi.begin(MIDI_CHANNEL_OMNI);
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
}

TEST(MidiInput, strayUndefinedOneByteParsing)
{
    SerialMock serial;
    MidiInterface midi(serial);

    static const unsigned rxSize = 13;
    static const byte rxData[rxSize] = {
        0xbb, 12, 0xf9, 34,
        12, 0,
        42, 0xfd, 127,
        0xf9,
        42, 0xfd, 0
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false); // Invalid, should not reset parser

    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      0);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      42);
    EXPECT_EQ(midi.getData2(),      127);

    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), false);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      42);
    EXPECT_EQ(midi.getData2(),      0);
}

TEST(MidiInput, strayUndefinedMultiByteParsing)
{
    typedef VariableSettings<false, false> Settings;
    typedef midi::MidiInterface<SerialMock, Settings> MultiByteMidiInterface;

    SerialMock serial;
    MultiByteMidiInterface midi(serial);

    static const unsigned rxSize = 4;
    static const byte rxData[rxSize] = {
        0xbb, 12, 0xf9, 34,
    };
    midi.begin(12);
    serial.mRxBuffer.write(rxData, rxSize);
    EXPECT_EQ(midi.read(), true);
    EXPECT_EQ(midi.getType(),       midi::ControlChange);
    EXPECT_EQ(midi.getChannel(),    12);
    EXPECT_EQ(midi.getData1(),      12);
    EXPECT_EQ(midi.getData2(),      34);
}

END_UNNAMED_NAMESPACE
