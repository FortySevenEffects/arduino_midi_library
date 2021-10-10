/*!
 *  @file       MIDI.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @author     Francois Best, lathoub
 *  @date       24/02/11
 *  @license    MIT - Copyright (c) 2015 Francois Best
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "midi_Defs.h"
#include "midi_Platform.h"
#include "midi_Settings.h"
#include "midi_Message.h"

#include "serialMIDI.h"

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

#define MIDI_LIBRARY_VERSION        0x050000
#define MIDI_LIBRARY_VERSION_MAJOR  5
#define MIDI_LIBRARY_VERSION_MINOR  0
#define MIDI_LIBRARY_VERSION_PATCH  0

/*! \brief The main class for MIDI handling.
It is templated over the type of serial port to provide abstraction from
the hardware interface, meaning you can use HardwareSerial, SoftwareSerial
or ak47's Uart classes. The only requirement is that the class implements
the begin, read, write and available methods.
 */
template<class Transport, class _Settings = DefaultSettings, class _Platform = DefaultPlatform>
class MidiInterface
{
public:
    typedef _Settings Settings;
    typedef _Platform Platform;
    typedef Message<Settings::SysExMaxSize> MidiMessage;

public:
    inline  MidiInterface(Transport&);
    inline ~MidiInterface();

public:
    MidiInterface& begin(Channel inChannel = 1);

    // -------------------------------------------------------------------------
    // MIDI Output

public:
    inline MidiInterface& sendNoteOn(DataByte inNoteNumber,
                           DataByte inVelocity,
                           Channel inChannel);

    inline MidiInterface& sendNoteOff(DataByte inNoteNumber,
                            DataByte inVelocity,
                            Channel inChannel);

    inline MidiInterface& sendProgramChange(DataByte inProgramNumber,
                                  Channel inChannel);

    inline MidiInterface& sendControlChange(DataByte inControlNumber,
                                  DataByte inControlValue,
                                  Channel inChannel);

    inline MidiInterface& sendPitchBend(int inPitchValue,    Channel inChannel);
    inline MidiInterface& sendPitchBend(double inPitchValue, Channel inChannel);

    inline MidiInterface& sendPolyPressure(DataByte inNoteNumber,
                                 DataByte inPressure,
                                 Channel inChannel) __attribute__ ((deprecated));

    inline MidiInterface& sendAfterTouch(DataByte inPressure,
                               Channel inChannel);
    inline MidiInterface& sendAfterTouch(DataByte inNoteNumber,
                               DataByte inPressure,
                               Channel inChannel);

    inline MidiInterface& sendSysEx(unsigned inLength,
                          const byte* inArray,
                          bool inArrayContainsBoundaries = false);

    inline MidiInterface& sendTimeCodeQuarterFrame(DataByte inTypeNibble,
                                         DataByte inValuesNibble);
    inline MidiInterface& sendTimeCodeQuarterFrame(DataByte inData);

    inline MidiInterface& sendSongPosition(unsigned inBeats);
    inline MidiInterface& sendSongSelect(DataByte inSongNumber);
    inline MidiInterface& sendTuneRequest();

    inline MidiInterface& sendCommon(MidiType inType, unsigned = 0);

    inline MidiInterface& sendClock()         { return sendRealTime(Clock); };
    inline MidiInterface& sendStart()         { return sendRealTime(Start); };
    inline MidiInterface& sendStop()          { return sendRealTime(Stop);  };
    inline MidiInterface& sendTick()          { return sendRealTime(Tick);  };
    inline MidiInterface& sendContinue()      { return sendRealTime(Continue);  };
    inline MidiInterface& sendActiveSensing() { return sendRealTime(ActiveSensing);  };
    inline MidiInterface& sendSystemReset()   { return sendRealTime(SystemReset);  };

    inline MidiInterface& sendRealTime(MidiType inType);

    inline MidiInterface& beginRpn(unsigned inNumber,
                         Channel inChannel);
    inline MidiInterface& sendRpnValue(unsigned inValue,
                             Channel inChannel);
    inline MidiInterface& sendRpnValue(byte inMsb,
                             byte inLsb,
                             Channel inChannel);
    inline MidiInterface& sendRpnIncrement(byte inAmount,
                                 Channel inChannel);
    inline MidiInterface& sendRpnDecrement(byte inAmount,
                                 Channel inChannel);
    inline MidiInterface& endRpn(Channel inChannel);

    inline MidiInterface& beginNrpn(unsigned inNumber,
                          Channel inChannel);
    inline MidiInterface& sendNrpnValue(unsigned inValue,
                              Channel inChannel);
    inline MidiInterface& sendNrpnValue(byte inMsb,
                              byte inLsb,
                              Channel inChannel);
    inline MidiInterface& sendNrpnIncrement(byte inAmount,
                                  Channel inChannel);
    inline MidiInterface& sendNrpnDecrement(byte inAmount,
                                  Channel inChannel);
    inline MidiInterface& endNrpn(Channel inChannel);

    inline MidiInterface& send(const MidiMessage&);

public:
    MidiInterface& send(MidiType inType,
              DataByte inData1,
              DataByte inData2,
              Channel inChannel);

    // -------------------------------------------------------------------------
    // MIDI Input

public:
    inline bool read();
    inline bool read(Channel inChannel);

public:
    inline MidiType getType() const;
    inline Channel  getChannel() const;
    inline DataByte getData1() const;
    inline DataByte getData2() const;
    inline const byte* getSysExArray() const;
    inline unsigned getSysExArrayLength() const;
    inline bool check() const;

public:
    inline Channel getInputChannel() const;
    inline MidiInterface& setInputChannel(Channel inChannel);

public:
    static inline MidiType getTypeFromStatusByte(byte inStatus);
    static inline Channel getChannelFromStatusByte(byte inStatus);
    static inline bool isChannelMessage(MidiType inType);

    // -------------------------------------------------------------------------
    // Input Callbacks

public:
    inline MidiInterface& setHandleMessage(void (*fptr)(const MidiMessage&)) { mMessageCallback = fptr; return *this; };
    inline MidiInterface& setHandleError(ErrorCallback fptr) { mErrorCallback = fptr; return *this; };
    inline MidiInterface& setHandleNoteOff(NoteOffCallback fptr) { mNoteOffCallback = fptr; return *this; };
    inline MidiInterface& setHandleNoteOn(NoteOnCallback fptr) { mNoteOnCallback = fptr; return *this; };
    inline MidiInterface& setHandleAfterTouchPoly(AfterTouchPolyCallback fptr) { mAfterTouchPolyCallback = fptr; return *this; };
    inline MidiInterface& setHandleControlChange(ControlChangeCallback fptr) { mControlChangeCallback = fptr; return *this; };
    inline MidiInterface& setHandleProgramChange(ProgramChangeCallback fptr) { mProgramChangeCallback = fptr; return *this; };
    inline MidiInterface& setHandleAfterTouchChannel(AfterTouchChannelCallback fptr) { mAfterTouchChannelCallback = fptr; return *this; };
    inline MidiInterface& setHandlePitchBend(PitchBendCallback fptr) { mPitchBendCallback = fptr; return *this; };
    inline MidiInterface& setHandleSystemExclusive(SystemExclusiveCallback fptr) { mSystemExclusiveCallback = fptr; return *this; };
    inline MidiInterface& setHandleTimeCodeQuarterFrame(TimeCodeQuarterFrameCallback fptr) { mTimeCodeQuarterFrameCallback = fptr; return *this; };
    inline MidiInterface& setHandleSongPosition(SongPositionCallback fptr) { mSongPositionCallback = fptr; return *this; };
    inline MidiInterface& setHandleSongSelect(SongSelectCallback fptr) { mSongSelectCallback = fptr; return *this; };
    inline MidiInterface& setHandleTuneRequest(TuneRequestCallback fptr) { mTuneRequestCallback = fptr; return *this; };
    inline MidiInterface& setHandleClock(ClockCallback fptr) { mClockCallback = fptr; return *this; };
    inline MidiInterface& setHandleStart(StartCallback fptr) { mStartCallback = fptr; return *this; };
    inline MidiInterface& setHandleTick(TickCallback fptr) { mTickCallback = fptr; return *this; };
    inline MidiInterface& setHandleContinue(ContinueCallback fptr) { mContinueCallback = fptr; return *this; };
    inline MidiInterface& setHandleStop(StopCallback fptr) { mStopCallback = fptr; return *this; };
    inline MidiInterface& setHandleActiveSensing(ActiveSensingCallback fptr) { mActiveSensingCallback = fptr; return *this; };
    inline MidiInterface& setHandleSystemReset(SystemResetCallback fptr) { mSystemResetCallback = fptr; return *this; };

    inline MidiInterface& disconnectCallbackFromType(MidiType inType);

private:
    void launchCallback();

    void (*mMessageCallback)(const MidiMessage& message) = nullptr;
    ErrorCallback mErrorCallback = nullptr;
    NoteOffCallback mNoteOffCallback = nullptr;
    NoteOnCallback mNoteOnCallback = nullptr;
    AfterTouchPolyCallback mAfterTouchPolyCallback = nullptr;
    ControlChangeCallback mControlChangeCallback = nullptr;
    ProgramChangeCallback mProgramChangeCallback = nullptr;
    AfterTouchChannelCallback mAfterTouchChannelCallback = nullptr;
    PitchBendCallback mPitchBendCallback = nullptr;
    SystemExclusiveCallback mSystemExclusiveCallback = nullptr;
    TimeCodeQuarterFrameCallback mTimeCodeQuarterFrameCallback = nullptr;
    SongPositionCallback mSongPositionCallback = nullptr;
    SongSelectCallback mSongSelectCallback = nullptr;
    TuneRequestCallback mTuneRequestCallback = nullptr;
    ClockCallback mClockCallback = nullptr;
    StartCallback mStartCallback = nullptr;
    TickCallback mTickCallback = nullptr;
    ContinueCallback mContinueCallback = nullptr;
    StopCallback mStopCallback = nullptr;
    ActiveSensingCallback mActiveSensingCallback = nullptr;
    SystemResetCallback mSystemResetCallback = nullptr;

    // -------------------------------------------------------------------------
    // MIDI Soft Thru

public:
    inline Thru::Mode getFilterMode() const;
    inline bool getThruState() const;

    inline MidiInterface& turnThruOn(Thru::Mode inThruFilterMode = Thru::Full);
    inline MidiInterface& turnThruOff();
    inline MidiInterface& setThruFilterMode(Thru::Mode inThruFilterMode);

private:
    void thruFilter(byte inChannel);

    // -------------------------------------------------------------------------
    // MIDI Parsing

private:
    bool parse();
    inline void handleNullVelocityNoteOnAsNoteOff();
    inline bool inputFilter(Channel inChannel);
    inline void resetInput();
    inline void updateLastSentTime();

    // -------------------------------------------------------------------------
    // Transport

public:
    Transport* getTransport() { return &mTransport; };

private:
    Transport& mTransport;

    // -------------------------------------------------------------------------
    // Internal variables

private:
    Channel         mInputChannel;
    StatusByte      mRunningStatus_RX;
    StatusByte      mRunningStatus_TX;
    byte            mPendingMessage[3];
    unsigned        mPendingMessageExpectedLength;
    unsigned        mPendingMessageIndex;
    unsigned        mCurrentRpnNumber;
    unsigned        mCurrentNrpnNumber;
    bool            mThruActivated  : 1;
    Thru::Mode      mThruFilterMode : 7;
    MidiMessage     mMessage;
    unsigned long   mLastMessageSentTime;
    unsigned long   mLastMessageReceivedTime;
    unsigned long   mSenderActiveSensingPeriodicity;
    bool            mReceiverActiveSensingActivated;
    int8_t          mLastError;

private:
    inline StatusByte getStatus(MidiType inType,
                                Channel inChannel) const;
};

// -----------------------------------------------------------------------------

unsigned encodeSysEx(const byte* inData,
                     byte* outSysEx,
                     unsigned inLength,
                     bool inFlipHeaderBits = false);
unsigned decodeSysEx(const byte* inSysEx,
                     byte* outData,
                     unsigned inLength,
                     bool inFlipHeaderBits = false);

END_MIDI_NAMESPACE

#include "MIDI.hpp"
