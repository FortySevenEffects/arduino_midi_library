#include "unit-tests.h"
#include "unit-tests_Settings.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
USING_NAMESPACE_UNIT_TESTS;

typedef test_mocks::SerialMock<32> SerialMock;
typedef midi::SerialMIDI<SerialMock> Transport;
typedef midi::MidiInterface<Transport> MidiInterface;

typedef std::vector<uint8_t> Buffer;

// --

TEST(MidiOutput, sendInvalid)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    midi.begin();
    midi.send(midi::NoteOn, 42, 42, 42);                // Invalid channel > OFF
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);

    midi.send(midi::InvalidType, 0, 0, 12);             // Invalid type
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);

    midi.send(midi::NoteOn, 12, 42, MIDI_CHANNEL_OMNI); // OMNI not allowed
    EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
}

TEST(MidiOutput, sendGenericSingle)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(3);

    midi.begin();
    midi.send(midi::NoteOn, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 47, 42}));
}

TEST(MidiOutput, sendGenericWithRunningStatus)
{
    typedef VariableSettings<true, false> Settings;
    typedef midi::MidiInterface<Transport, Settings> RsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;
    buffer.resize(5);

    midi.begin();
    EXPECT_EQ(RsMidiInterface::Settings::UseRunningStatus, true);
    EXPECT_EQ(serial.mTxBuffer.isEmpty(), true);
    midi.send(midi::NoteOn, 47, 42, 12);
    midi.send(midi::NoteOn, 42, 47, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);
    serial.mTxBuffer.read(&buffer[0], 5);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 47, 42, 42, 47}));
}

TEST(MidiOutput, sendGenericWithoutRunningStatus)
{
    typedef VariableSettings<false, true> Settings; // No running status
    typedef midi::MidiInterface<Transport, Settings> NoRsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    NoRsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;
    buffer.resize(6);

    // Same status byte
    midi.begin();
    EXPECT_EQ(MidiInterface::Settings::UseRunningStatus, false);
    EXPECT_EQ(serial.mTxBuffer.isEmpty(), true);
    midi.send(midi::NoteOn, 47, 42, 12);
    midi.send(midi::NoteOn, 42, 47, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 47, 42, 0x9b, 42, 47}));

    // Different status byte
    midi.begin();
    midi.send(midi::NoteOn,  47, 42, 12);
    midi.send(midi::NoteOff, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 47, 42, 0x8b, 47, 42}));
}

TEST(MidiOutput, sendGenericBreakingRunningStatus)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.send(midi::NoteOn,  47, 42, 12);
    midi.send(midi::NoteOff, 47, 42, 12);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 47, 42, 0x8b, 47, 42}));
}

TEST(MidiOutput, sendGenericRealTimeShortcut)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.send(midi::Clock,          47, 42, 12);
    midi.send(midi::Start,          47, 42, 12);
    midi.send(midi::Continue,       47, 42, 12);
    midi.send(midi::Stop,           47, 42, 12);
    midi.send(midi::ActiveSensing,  47, 42, 12);
    midi.send(midi::SystemReset,    47, 42, 12);

    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0xf8, 0xfa, 0xfb, 0xfc, 0xfe, 0xff}));
}

// --

TEST(MidiOutput, sendNoteOn)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendNoteOn(10, 11, 12);
    midi.sendNoteOn(12, 13, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0x9b, 10, 11, 0x93, 12, 13}));
}

TEST(MidiOutput, sendNoteOff)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendNoteOff(10, 11, 12);
    midi.sendNoteOff(12, 13, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0x8b, 10, 11, 0x83, 12, 13}));
}

TEST(MidiOutput, sendProgramChange)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(4);

    midi.begin();
    midi.sendProgramChange(42, 12);
    midi.sendProgramChange(47, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
    serial.mTxBuffer.read(&buffer[0], 4);
    EXPECT_THAT(buffer, ElementsAreArray({0xcb, 42, 0xc3, 47}));
}

TEST(MidiOutput, sendControlChange)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendControlChange(42, 12, 12);
    midi.sendControlChange(47, 12, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0xbb, 42, 12, 0xb3, 47, 12}));
}

TEST(MidiOutput, sendPitchBend)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    // Int signature - arbitrary values
    {
        buffer.clear();
        buffer.resize(9);

        midi.begin();
        midi.sendPitchBend(0,    12);
        midi.sendPitchBend(100,  4);
        midi.sendPitchBend(-100, 7);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 9);
        serial.mTxBuffer.read(&buffer[0], 9);
        EXPECT_THAT(buffer, ElementsAreArray({0xeb, 0x00, 0x40,
                                              0xe3, 0x64, 0x40,
                                              0xe6, 0x1c, 0x3f}));
    }
    // Int signature - min/max
    {
        buffer.clear();
        buffer.resize(9);

        midi.begin();
        midi.sendPitchBend(0, 12);
        midi.sendPitchBend(MIDI_PITCHBEND_MAX, 4);
        midi.sendPitchBend(MIDI_PITCHBEND_MIN, 7);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 9);
        serial.mTxBuffer.read(&buffer[0], 9);
        EXPECT_THAT(buffer, ElementsAreArray({0xeb, 0x00, 0x40,
                                              0xe3, 0x7f, 0x7f,
                                              0xe6, 0x00, 0x00}));
    }
    // Float signature
    {
        buffer.clear();
        buffer.resize(9);

        midi.begin();
        midi.sendPitchBend(0.0,  12);
        midi.sendPitchBend(1.0,  4);
        midi.sendPitchBend(-1.0, 7);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 9);
        serial.mTxBuffer.read(&buffer[0], 9);
        EXPECT_THAT(buffer, ElementsAreArray({0xeb, 0x00, 0x40,
                                              0xe3, 0x7f, 0x7f,
                                              0xe6, 0x00, 0x00}));
    }
}

TEST(MidiOutput, sendPolyPressure)
{
    // Note: sendPolyPressure is deprecated in favor of sendAfterTouch, which
    // now supports both mono and poly AfterTouch messages.
    // This test is kept for coverage until removal of sendPolyPressure.

    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendPolyPressure(42, 12, 12);
    midi.sendPolyPressure(47, 12, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0xab, 42, 12, 0xa3, 47, 12}));
}

TEST(MidiOutput, sendAfterTouchMono)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(4);

    midi.begin();
    midi.sendAfterTouch(42, 12);
    midi.sendAfterTouch(47, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
    serial.mTxBuffer.read(&buffer[0], 4);
    EXPECT_THAT(buffer, ElementsAreArray({0xdb, 42, 0xd3, 47}));
}

TEST(MidiOutput, sendAfterTouchPoly)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendAfterTouch(42, 12, 12);
    midi.sendAfterTouch(47, 12, 4);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0xab, 42, 12, 0xa3, 47, 12}));
}

TEST(MidiOutput, sendSysEx)
{
    typedef test_mocks::SerialMock<1024> LargeSerialMock;
    typedef midi::SerialMIDI<LargeSerialMock> LargeTransport;
    typedef midi::MidiInterface<LargeTransport> LargeMidiInterface;

    LargeSerialMock serial;
    LargeTransport transport(serial);
    LargeMidiInterface midi((LargeTransport&)transport);
    
    Buffer buffer;

    // Short frame
    {
        static const char* frame = "Hello, World!";
        static const int frameLength = strlen(frame);
        static const byte expected[] = {
            0xf0,
            'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!',
            0xf7,
        };

        buffer.clear();
        buffer.resize(frameLength + 2);

        midi.begin();
        midi.sendSysEx(frameLength, reinterpret_cast<const byte*>(frame), false);
        EXPECT_EQ(serial.mTxBuffer.getLength(), frameLength + 2);
        serial.mTxBuffer.read(&buffer[0], frameLength + 2);
        EXPECT_THAT(buffer, ElementsAreArray(expected));
    }
    // Long frame
    {
        static const char* frame = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin maximus dui a massa maximus, a vestibulum mi venenatis. Cras sit amet ex id velit suscipit pharetra eget a turpis. Phasellus interdum metus ac sagittis cursus. Nam quis est at nisl ullamcorper egestas pulvinar eu erat. Duis a elit dignissim, vestibulum eros vel, tempus nisl. Aenean turpis nunc, cursus vel lacinia non, pharetra eget sapien. Duis condimentum, lacus at pulvinar tempor, leo libero volutpat nisl, eget porttitor lorem mi sed magna. Duis dictum, massa vel euismod interdum, lorem mi egestas elit, hendrerit tincidunt est arcu a libero. Interdum et malesuada fames ac ante ipsum primis in faucibus. Curabitur vehicula magna libero, at rhoncus sem ornare a. In elementum, elit et congue pulvinar, massa velit commodo velit, non elementum purus ligula eget lacus. Donec efficitur nisi eu ultrices efficitur. Donec neque dui, ullamcorper id molestie quis, consequat sit amet ligula.";
        static const int frameLength = strlen(frame);
        static const byte expected[] = {
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

        buffer.clear();
        buffer.resize(frameLength + 2);

        midi.begin();
        midi.sendSysEx(frameLength, reinterpret_cast<const byte*>(frame), false);
        EXPECT_EQ(serial.mTxBuffer.getLength(), frameLength + 2);
        serial.mTxBuffer.read(&buffer[0], frameLength + 2);
        EXPECT_THAT(buffer, ElementsAreArray(expected));
    }
    // With boundaries included
    {
        static const byte frame[] = {
            0xf0, 12, 17, 42, 47, 0xf7
        };

        buffer.clear();
        buffer.resize(6);

        midi.begin();
        midi.sendSysEx(6, frame, true);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
        serial.mTxBuffer.read(&buffer[0], 6);
        EXPECT_THAT(buffer, ElementsAreArray(frame));
    }
}

TEST(MidiOutput, sendTimeCodeQuarterFrame)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    // Separate Nibbles
    {
        buffer.clear();
        buffer.resize(4);

        midi.begin();
        midi.sendTimeCodeQuarterFrame(0x05, 0x0a);
        midi.sendTimeCodeQuarterFrame(0xff, 0xff);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
        serial.mTxBuffer.read(&buffer[0], 4);
        EXPECT_THAT(buffer, ElementsAreArray({0xf1, 0x5a,
                                              0xf1, 0x7f}));
    }
    // Pre-encoded nibbles
    {
        buffer.clear();
        buffer.resize(4);

        midi.begin();
        midi.sendTimeCodeQuarterFrame(12);
        midi.sendTimeCodeQuarterFrame(42);
        EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
        serial.mTxBuffer.read(&buffer[0], 4);
        EXPECT_THAT(buffer, ElementsAreArray({0xf1, 0x0c,
                                              0xf1, 0x2a}));
    }
}

TEST(MidiOutput, sendSongPosition)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(6);

    midi.begin();
    midi.sendSongPosition(1234);
    midi.sendSongPosition(4321);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({0xf2, 0x52, 0x09,
                                          0xf2, 0x61, 0x21}));
}

TEST(MidiOutput, sendSongSelect)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(4);

    midi.begin();
    midi.sendSongSelect(12);
    midi.sendSongSelect(42);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
    serial.mTxBuffer.read(&buffer[0], 4);
    EXPECT_THAT(buffer, ElementsAreArray({0xf3, 12, 0xf3, 42}));
}

TEST(MidiOutput, sendTuneRequest)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;
    buffer.resize(1);

    midi.begin();
    midi.sendTuneRequest();
    EXPECT_EQ(serial.mTxBuffer.getLength(), 1);
    serial.mTxBuffer.read(&buffer[0], 1);
    EXPECT_THAT(buffer, ElementsAreArray({0xf6}));
}

TEST(MidiOutput, sendRealTime)
{
    SerialMock serial;
    Transport transport(serial);
    MidiInterface midi((Transport&)transport);

    Buffer buffer;

    // Test valid RealTime messages
    {
        buffer.clear();
        buffer.resize(6);

        midi.begin();
        midi.sendRealTime(midi::Clock);
        midi.sendRealTime(midi::Start);
        midi.sendRealTime(midi::Continue);
        midi.sendRealTime(midi::Stop);
        midi.sendRealTime(midi::ActiveSensing);
        midi.sendRealTime(midi::SystemReset);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
        serial.mTxBuffer.read(&buffer[0], 6);
        EXPECT_THAT(buffer, ElementsAreArray({
            0xf8, 0xfa, 0xfb, 0xfc, 0xfe, 0xff
        }));
    }
    // Test invalid messages
    {
        midi.begin();
        midi.sendRealTime(midi::InvalidType);
        midi.sendRealTime(midi::NoteOff);
        midi.sendRealTime(midi::NoteOn);
        midi.sendRealTime(midi::AfterTouchPoly);
        midi.sendRealTime(midi::ControlChange);
        midi.sendRealTime(midi::ProgramChange);
        midi.sendRealTime(midi::AfterTouchChannel);
        midi.sendRealTime(midi::PitchBend);
        midi.sendRealTime(midi::SystemExclusive);
        midi.sendRealTime(midi::TimeCodeQuarterFrame);
        midi.sendRealTime(midi::SongPosition);
        midi.sendRealTime(midi::SongSelect);
        midi.sendRealTime(midi::TuneRequest);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 0);
    }
}

TEST(MidiOutput, RPN)
{
    typedef VariableSettings<true, true> Settings;
    typedef midi::MidiInterface<Transport, Settings> RsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    // 14-bit Value Single Frame
    {
        buffer.clear();
        buffer.resize(13);

        midi.begin();
        midi.beginRpn(1242, 12);
        midi.sendRpnValue(12345, 12);
        midi.endRpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 13);
        serial.mTxBuffer.read(&buffer[0], 13);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x64, 0x5a,
                                              0x65, 0x09,
                                              0x06, 0x60,
                                              0x26, 0x39,
                                              0x64, 0x7f,
                                              0x65, 0x7f}));
    }
    // MSB/LSB Single Frame
    {
        buffer.clear();
        buffer.resize(13);

        midi.begin();
        midi.beginRpn(1242, 12);
        midi.sendRpnValue(12, 42, 12);
        midi.endRpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 13);
        serial.mTxBuffer.read(&buffer[0], 13);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x64, 0x5a,
                                              0x65, 0x09,
                                              0x06, 0x0c,
                                              0x26, 0x2a,
                                              0x64, 0x7f,
                                              0x65, 0x7f}));
    }
    // Increment Single Frame
    {
        buffer.clear();
        buffer.resize(11);

        midi.begin();
        midi.beginRpn(1242, 12);
        midi.sendRpnIncrement(42, 12);
        midi.endRpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 11);
        serial.mTxBuffer.read(&buffer[0], 11);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x64, 0x5a,
                                              0x65, 0x09,
                                              0x60, 0x2a,
                                              0x64, 0x7f,
                                              0x65, 0x7f}));
    }
    // Decrement Single Frame
    {
        buffer.clear();
        buffer.resize(11);

        midi.begin();
        midi.beginRpn(1242, 12);
        midi.sendRpnDecrement(42, 12);
        midi.endRpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 11);
        serial.mTxBuffer.read(&buffer[0], 11);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x64, 0x5a,
                                              0x65, 0x09,
                                              0x61, 0x2a,
                                              0x64, 0x7f,
                                              0x65, 0x7f}));
    }
    // Multi Frame
    {
        buffer.clear();
        buffer.resize(21);

        midi.begin();
        midi.beginRpn(1242, 12);
        midi.sendRpnValue(12345, 12);
        midi.sendRpnValue(12, 42, 12);
        midi.sendRpnIncrement(42, 12);
        midi.sendRpnDecrement(42, 12);
        midi.endRpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 21);
        serial.mTxBuffer.read(&buffer[0], 21);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x64, 0x5a,
                                              0x65, 0x09,
                                              0x06, 0x60,
                                              0x26, 0x39,
                                              0x06, 0x0c,
                                              0x26, 0x2a,
                                              0x60, 0x2a,
                                              0x61, 0x2a,
                                              0x64, 0x7f,
                                              0x65, 0x7f}));
    }
}

TEST(MidiOutput, NRPN)
{
    typedef VariableSettings<true, true> Settings;
    typedef midi::MidiInterface<Transport, Settings> RsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    // 14-bit Value Single Frame
    {
        buffer.clear();
        buffer.resize(13);

        midi.begin();
        midi.beginNrpn(1242, 12);
        midi.sendNrpnValue(12345, 12);
        midi.endNrpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 13);
        serial.mTxBuffer.read(&buffer[0], 13);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x62, 0x5a,
                                              0x63, 0x09,
                                              0x06, 0x60,
                                              0x26, 0x39,
                                              0x62, 0x7f,
                                              0x63, 0x7f}));
    }
    // MSB/LSB Single Frame
    {
        buffer.clear();
        buffer.resize(13);

        midi.begin();
        midi.beginNrpn(1242, 12);
        midi.sendNrpnValue(12, 42, 12);
        midi.endNrpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 13);
        serial.mTxBuffer.read(&buffer[0], 13);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x62, 0x5a,
                                              0x63, 0x09,
                                              0x06, 0x0c,
                                              0x26, 0x2a,
                                              0x62, 0x7f,
                                              0x63, 0x7f}));
    }
    // Increment Single Frame
    {
        buffer.clear();
        buffer.resize(11);

        midi.begin();
        midi.beginNrpn(1242, 12);
        midi.sendNrpnIncrement(42, 12);
        midi.endNrpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 11);
        serial.mTxBuffer.read(&buffer[0], 11);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x62, 0x5a,
                                              0x63, 0x09,
                                              0x60, 0x2a,
                                              0x62, 0x7f,
                                              0x63, 0x7f}));
    }
    // Decrement Single Frame
    {
        buffer.clear();
        buffer.resize(11);

        midi.begin();
        midi.beginNrpn(1242, 12);
        midi.sendNrpnDecrement(42, 12);
        midi.endNrpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 11);
        serial.mTxBuffer.read(&buffer[0], 11);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x62, 0x5a,
                                              0x63, 0x09,
                                              0x61, 0x2a,
                                              0x62, 0x7f,
                                              0x63, 0x7f}));
    }
    // Multi Frame
    {
        buffer.clear();
        buffer.resize(21);

        midi.begin();
        midi.beginNrpn(1242, 12);
        midi.sendNrpnValue(12345, 12);
        midi.sendNrpnValue(12, 42, 12);
        midi.sendNrpnIncrement(42, 12);
        midi.sendNrpnDecrement(42, 12);
        midi.endNrpn(12);

        EXPECT_EQ(serial.mTxBuffer.getLength(), 21);
        serial.mTxBuffer.read(&buffer[0], 21);
        EXPECT_THAT(buffer, ElementsAreArray({0xbb,
                                              0x62, 0x5a,
                                              0x63, 0x09,
                                              0x06, 0x60,
                                              0x26, 0x39,
                                              0x06, 0x0c,
                                              0x26, 0x2a,
                                              0x60, 0x2a,
                                              0x61, 0x2a,
                                              0x62, 0x7f,
                                              0x63, 0x7f}));
    }
}

TEST(MidiOutput, runningStatusCancellation)
{
    typedef VariableSettings<true, false> Settings;
    typedef midi::MidiInterface<Transport, Settings> RsMidiInterface;

    SerialMock serial;
    Transport transport(serial);
    RsMidiInterface midi((Transport&)transport);
    
    Buffer buffer;

    static const unsigned sysExLength = 13;
    static const byte sysEx[sysExLength] = {
        'H','e','l','l','o',',',' ','W','o','r','l','d','!'
    };

    midi.begin();

    midi.sendNoteOn(12, 34, 1);
    midi.sendNoteOn(56, 78, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);

    buffer.clear();
    buffer.resize(5);
    serial.mTxBuffer.read(&buffer[0], 5);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x90, 12, 34, 56, 78
    }));

    midi.sendRealTime(midi::Clock);     // Should not reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 3);
    buffer.clear();
    buffer.resize(3);
    serial.mTxBuffer.read(&buffer[0], 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0xf8, 12, 34
    }));

    midi.sendSysEx(sysExLength, sysEx); // Should reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 18);
    buffer.clear();
    buffer.resize(18);
    serial.mTxBuffer.read(&buffer[0], 18);
    {
        static const byte expected[] = {
            0xf0, 'H','e','l','l','o',',',' ','W','o','r','l','d','!', 0xf7,
            0x90, 12, 34
        };
        EXPECT_THAT(buffer, ElementsAreArray(expected));
    }

    midi.sendTimeCodeQuarterFrame(42);  // Should reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);
    buffer.clear();
    buffer.resize(5);
    serial.mTxBuffer.read(&buffer[0], 5);
    EXPECT_THAT(buffer, ElementsAreArray({
        0xf1, 42,
        0x90, 12, 34
    }));

    midi.sendSongPosition(42);          // Should reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 6);
    buffer.clear();
    buffer.resize(6);
    serial.mTxBuffer.read(&buffer[0], 6);
    EXPECT_THAT(buffer, ElementsAreArray({
        0xf2, 42, 0,
        0x90, 12, 34
    }));

    midi.sendSongSelect(42);            // Should reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 5);
    buffer.clear();
    buffer.resize(5);
    serial.mTxBuffer.read(&buffer[0], 5);
    EXPECT_THAT(buffer, ElementsAreArray({
        0xf3, 42,
        0x90, 12, 34
    }));

    midi.sendTuneRequest();             // Should reset running status.
    midi.sendNoteOn(12, 34, 1);
    EXPECT_EQ(serial.mTxBuffer.getLength(), 4);
    buffer.clear();
    buffer.resize(4);
    serial.mTxBuffer.read(&buffer[0], 4);
    EXPECT_THAT(buffer, ElementsAreArray({
        0xf6,
        0x90, 12, 34
    }));
}

END_UNNAMED_NAMESPACE
