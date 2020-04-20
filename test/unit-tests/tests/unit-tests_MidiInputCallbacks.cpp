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

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

typedef test_mocks::SerialMock<256> SerialMock;
typedef midi::SerialMIDI<SerialMock> Transport;

typedef VariableSysExSettings<256> Settings;
typedef midi::MidiInterface<Transport, Settings> MidiInterface;

MidiInterface* midi;

class MidiInputCallbacks : public Test
{
public:
    MidiInputCallbacks()
        : mTransport(mSerial)
        , mMidi(mTransport)
    {
    }
    virtual ~MidiInputCallbacks()
    {
    }

protected:
    virtual void SetUp()
    {
        midi = &mMidi;
    }

    virtual void TearDown()
    {
        midi = nullptr;
    }

protected:
    SerialMock      mSerial;
    Transport       mTransport;
    MidiInterface   mMidi;
};

// --

void handleNoteOn(byte inChannel, byte inPitch, byte inVelocity)
{
    EXPECT_NE(midi, nullptr);
    midi->sendNoteOn(inPitch, inVelocity, inChannel);
}

TEST_F(MidiInputCallbacks, noteOn)
{
    mMidi.setHandleNoteOn(handleNoteOn);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x9b, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::NoteOn);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));

    // Test null velocity note on
    EXPECT_EQ(MidiInterface::Settings::HandleNullVelocityNoteOnAsNoteOff, true);
    mSerial.mRxBuffer.write(0x9b);
    mSerial.mRxBuffer.write(12);
    mSerial.mRxBuffer.write(0);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::NoteOff);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 0);
}

// --

void handleNoteOff(byte inChannel, byte inPitch, byte inVelocity)
{
    EXPECT_NE(midi, nullptr);
    midi->sendNoteOff(inPitch, inVelocity, inChannel);
}

TEST_F(MidiInputCallbacks, noteOff)
{
    mMidi.setHandleNoteOff(handleNoteOff);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0x8b, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::NoteOff);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));

    // Test null velocity note on
    EXPECT_EQ(MidiInterface::Settings::HandleNullVelocityNoteOnAsNoteOff, true);
    mSerial.mRxBuffer.write(0x9b);
    mSerial.mRxBuffer.write(12);
    mSerial.mRxBuffer.write(0);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::NoteOff);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ElementsAreArray({
        0x8b, 12, 0
    }));
}

// --



void handleAfterTouchPoly(byte inChannel, byte inNote, byte inValue)
{
    EXPECT_NE(midi, nullptr);
    midi->sendAfterTouch(inNote, inValue, inChannel);
}

TEST_F(MidiInputCallbacks, afterTouchPoly)
{
    mMidi.setHandleAfterTouchPoly(handleAfterTouchPoly);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0xab, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::AfterTouchPoly);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleControlChange(byte inChannel, byte inNumber, byte inValue)
{
    EXPECT_NE(midi, nullptr);
    midi->sendControlChange(inNumber, inValue, inChannel);
}

TEST_F(MidiInputCallbacks, controlChange)
{
    mMidi.setHandleControlChange(handleControlChange);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0xbb, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::ControlChange);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleProgramChange(byte inChannel, byte inNumber)
{
    EXPECT_NE(midi, nullptr);
    midi->sendProgramChange(inNumber, inChannel);
}

TEST_F(MidiInputCallbacks, programChange)
{
    mMidi.setHandleProgramChange(handleProgramChange);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 2;
    static const byte rxData[rxSize] = { 0xcb, 12 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::ProgramChange);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 2);
    byte buffer[2] = { 0 };
    mSerial.mTxBuffer.read(buffer, 2);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleAfterTouchChannel(byte inChannel, byte inPressure)
{
    EXPECT_NE(midi, nullptr);
    midi->sendAfterTouch(inPressure, inChannel);
}


TEST_F(MidiInputCallbacks, afterTouchChannel)
{
    mMidi.setHandleAfterTouchChannel(handleAfterTouchChannel);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 2;
    static const byte rxData[rxSize] = { 0xdb, 12 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::AfterTouchChannel);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 2);
    byte buffer[2] = { 0 };
    mSerial.mTxBuffer.read(buffer, 2);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handlePitchBend(byte inChannel, int inValue)
{
    EXPECT_NE(midi, nullptr);
    midi->sendPitchBend(inValue, inChannel);
}

TEST_F(MidiInputCallbacks, pitchBend)
{
    mMidi.setHandlePitchBend(handlePitchBend);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0xeb, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::PitchBend);
    EXPECT_EQ(mMidi.getChannel(),    12);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleSysEx(byte* inData, unsigned inSize)
{
    EXPECT_NE(midi, nullptr);
    midi->sendSysEx(inSize, inData, true);
}

TEST_F(MidiInputCallbacks, sysEx)
{
    mMidi.setHandleSystemExclusive(handleSysEx);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 15;
    static const byte rxData[rxSize] = {
        0xf0, 'H','e','l','l','o',',',' ','W','o','r','l','d','!', 0xf7
    };
    mSerial.mRxBuffer.write(rxData, rxSize);

    for (unsigned i = 0; i < rxSize - 1; ++i)
    {
        EXPECT_EQ(mMidi.read(), false);
    }
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),              midi::SystemExclusive);
    EXPECT_EQ(mMidi.getChannel(),           0);
    EXPECT_EQ(mMidi.getSysExArrayLength(),  rxSize);

    EXPECT_EQ(unsigned(mSerial.mTxBuffer.getLength()), rxSize);
    const std::vector<byte> sysExData(mMidi.getSysExArray(),
                                      mMidi.getSysExArray() + rxSize);
    EXPECT_THAT(sysExData, ElementsAreArray(rxData));
}

TEST_F(MidiInputCallbacks, sysExLong)
{
    mMidi.setHandleSystemExclusive(handleSysEx);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 210;
    static const byte rxData[rxSize] = {
        0xf0,
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
            'H','e','l','l','o',',',' ','W','o','r','l','d','!',
        0xf7
    };
    mSerial.mRxBuffer.write(rxData, rxSize);

    for (unsigned i = 0; i < rxSize - 1; ++i)
    {
        EXPECT_EQ(mMidi.read(), false);
    }
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),              midi::SystemExclusive);
    EXPECT_EQ(mMidi.getChannel(),           0);
    EXPECT_EQ(mMidi.getSysExArrayLength(),  rxSize);

    EXPECT_EQ(unsigned(mSerial.mTxBuffer.getLength()), rxSize);
    const std::vector<byte> sysExData(mMidi.getSysExArray(),
                                      mMidi.getSysExArray() + rxSize);
    EXPECT_THAT(sysExData, ElementsAreArray(rxData));
}

// --

void handleMtcQuarterFrame(byte inData)
{
    EXPECT_NE(midi, nullptr);
    midi->sendTimeCodeQuarterFrame(inData);
}

TEST_F(MidiInputCallbacks, mtcQuarterFrame)
{
    mMidi.setHandleTimeCodeQuarterFrame(handleMtcQuarterFrame);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 2;
    static const byte rxData[rxSize] = { 0xf1, 12 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::TimeCodeQuarterFrame);
    EXPECT_EQ(mMidi.getChannel(),    0);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 2);
    byte buffer[2] = { 0 };
    mSerial.mTxBuffer.read(buffer, 2);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleSongPosition(unsigned inBeats)
{
    EXPECT_NE(midi, nullptr);
    midi->sendSongPosition(inBeats);
}

TEST_F(MidiInputCallbacks, songPosition)
{
    mMidi.setHandleSongPosition(handleSongPosition);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 3;
    static const byte rxData[rxSize] = { 0xf2, 12, 34 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::SongPosition);
    EXPECT_EQ(mMidi.getChannel(),    0);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      34);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 3);
    byte buffer[3] = { 0 };
    mSerial.mTxBuffer.read(buffer, 3);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleSongSelect(byte inSongNumber)
{
    EXPECT_NE(midi, nullptr);
    midi->sendSongSelect(inSongNumber);
}

TEST_F(MidiInputCallbacks, songSelect)
{
    mMidi.setHandleSongSelect(handleSongSelect);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 2;
    static const byte rxData[rxSize] = { 0xf3, 12 };
    mSerial.mRxBuffer.write(rxData, rxSize);

    EXPECT_EQ(mMidi.read(), false);
    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::SongSelect);
    EXPECT_EQ(mMidi.getChannel(),    0);
    EXPECT_EQ(mMidi.getData1(),      12);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 2);
    byte buffer[2] = { 0 };
    mSerial.mTxBuffer.read(buffer, 2);
    EXPECT_THAT(buffer, ContainerEq(rxData));
}

// --

void handleTuneRequest()
{
    EXPECT_NE(midi, nullptr);
    midi->sendTuneRequest();
}

TEST_F(MidiInputCallbacks, tuneRequest)
{
    mMidi.setHandleTuneRequest(handleTuneRequest);
    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    mSerial.mRxBuffer.write(0xf6);

    EXPECT_EQ(mMidi.read(), true);
    EXPECT_EQ(mMidi.getType(),       midi::TuneRequest);
    EXPECT_EQ(mMidi.getChannel(),    0);
    EXPECT_EQ(mMidi.getData1(),      0);
    EXPECT_EQ(mMidi.getData2(),      0);

    EXPECT_EQ(mSerial.mTxBuffer.getLength(), 1);
    EXPECT_EQ(mSerial.mTxBuffer.read(), 0xf6);
}

// --

void handleClock()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::Clock);
}
void handleStart()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::Start);
}
void handleContinue()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::Continue);
}
void handleStop()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::Stop);
}
void handleActiveSensing()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::ActiveSensing);
}
void handleSystemReset()
{
    EXPECT_NE(midi, nullptr);
    midi->sendRealTime(midi::SystemReset);
}

TEST_F(MidiInputCallbacks, realTime)
{
    mMidi.setHandleClock(handleClock);
    mMidi.setHandleStart(handleStart);
    mMidi.setHandleContinue(handleContinue);
    mMidi.setHandleStop(handleStop);
    mMidi.setHandleActiveSensing(handleActiveSensing);
    mMidi.setHandleSystemReset(handleSystemReset);

    mMidi.begin(MIDI_CHANNEL_OMNI);
    mMidi.turnThruOff();

    static const unsigned rxSize = 6;
    static const byte rxData[rxSize] = {
        0xf8, 0xfa, 0xfb, 0xfc, 0xfe, 0xff
    };
    mSerial.mRxBuffer.write(rxData, rxSize);
    static const midi::MidiType types[rxSize] = {
        midi::Clock,
        midi::Start,
        midi::Continue,
        midi::Stop,
        midi::ActiveSensing,
        midi::SystemReset,
    };

    for (unsigned i = 0; i < rxSize; ++i)
    {
        EXPECT_EQ(mMidi.read(),         true);
        EXPECT_EQ(mMidi.getType(),      types[i]);
        EXPECT_EQ(mMidi.getChannel(),   0);
        EXPECT_EQ(mMidi.getData1(),     0);
        EXPECT_EQ(mMidi.getData2(),     0);
        EXPECT_EQ(mSerial.mTxBuffer.getLength(), 1);

        const byte read = mSerial.mTxBuffer.read();
        EXPECT_EQ(read, rxData[i]);
        EXPECT_EQ(read, types[i]);
    }
}

END_UNNAMED_NAMESPACE
