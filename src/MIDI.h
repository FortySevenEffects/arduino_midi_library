/*!
 *  @file       MIDI.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @version    4.2
 *  @author     Francois Best
 *  @date       24/02/11
 *  @license    GPL v3.0 - Copyright Forty Seven Effects 2014
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "midi_Defs.h"
#include "midi_Settings.h"
#include "midi_Message.h"

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

/*! \brief The main class for MIDI handling.
It is templated over the type of serial port to provide abstraction from
the hardware interface, meaning you can use HardwareSerial, SoftwareSerial
or ak47's Uart classes. The only requirement is that the class implements
the begin, read, write and available methods.
 */
template<class SerialPort, class Settings = DefaultSettings>
class MidiInterface
{
public:
    inline  MidiInterface(SerialPort& inSerial);
    inline ~MidiInterface();

public:
    void begin(Channel inChannel = 1);

    // -------------------------------------------------------------------------
    // MIDI Output

public:
    inline void sendNoteOn(DataByte inNoteNumber,
                           DataByte inVelocity,
                           Channel inChannel);

    inline void sendNoteOff(DataByte inNoteNumber,
                            DataByte inVelocity,
                            Channel inChannel);

    inline void sendProgramChange(DataByte inProgramNumber,
                                  Channel inChannel);

    inline void sendControlChange(DataByte inControlNumber,
                                  DataByte inControlValue,
                                  Channel inChannel);

    inline void sendPitchBend(int inPitchValue,    Channel inChannel);
    inline void sendPitchBend(double inPitchValue, Channel inChannel);

    inline void sendPolyPressure(DataByte inNoteNumber,
                                 DataByte inPressure,
                                 Channel inChannel);

    inline void sendAfterTouch(DataByte inPressure,
                               Channel inChannel);

    inline void sendSysEx(unsigned inLength,
                          const byte* inArray,
                          bool inArrayContainsBoundaries = false);

    inline void sendTimeCodeQuarterFrame(DataByte inTypeNibble,
                                         DataByte inValuesNibble);
    inline void sendTimeCodeQuarterFrame(DataByte inData);

    inline void sendSongPosition(unsigned inBeats);
    inline void sendSongSelect(DataByte inSongNumber);
    inline void sendTuneRequest();
    inline void sendRealTime(MidiType inType);

public:
    void send(MidiType inType,
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
    inline void setInputChannel(Channel inChannel);

public:
    static inline MidiType getTypeFromStatusByte(byte inStatus);
    static inline Channel getChannelFromStatusByte(byte inStatus);
	static inline bool isChannelMessage(MidiType inType);


    // -------------------------------------------------------------------------
    // Input Callbacks

public:
    inline void setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity));
    inline void setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity));
    inline void setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure));
    inline void setHandleControlChange(void (*fptr)(byte channel, byte number, byte value));
    inline void setHandleProgramChange(void (*fptr)(byte channel, byte number));
    inline void setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure));
    inline void setHandlePitchBend(void (*fptr)(byte channel, int bend));
    inline void setHandleSystemExclusive(void (*fptr)(byte * array, unsigned size));
    inline void setHandleTimeCodeQuarterFrame(void (*fptr)(byte data));
    inline void setHandleSongPosition(void (*fptr)(unsigned beats));
    inline void setHandleSongSelect(void (*fptr)(byte songnumber));
    inline void setHandleTuneRequest(void (*fptr)(void));
    inline void setHandleClock(void (*fptr)(void));
    inline void setHandleStart(void (*fptr)(void));
    inline void setHandleContinue(void (*fptr)(void));
    inline void setHandleStop(void (*fptr)(void));
    inline void setHandleActiveSensing(void (*fptr)(void));
    inline void setHandleSystemReset(void (*fptr)(void));

    inline void disconnectCallbackFromType(MidiType inType);

private:
    void launchCallback();

    void (*mNoteOffCallback)(byte channel, byte note, byte velocity);
    void (*mNoteOnCallback)(byte channel, byte note, byte velocity);
    void (*mAfterTouchPolyCallback)(byte channel, byte note, byte velocity);
    void (*mControlChangeCallback)(byte channel, byte, byte);
    void (*mProgramChangeCallback)(byte channel, byte);
    void (*mAfterTouchChannelCallback)(byte channel, byte);
    void (*mPitchBendCallback)(byte channel, int);
    void (*mSystemExclusiveCallback)(byte * array, unsigned size);
    void (*mTimeCodeQuarterFrameCallback)(byte data);
    void (*mSongPositionCallback)(unsigned beats);
    void (*mSongSelectCallback)(byte songnumber);
    void (*mTuneRequestCallback)(void);
    void (*mClockCallback)(void);
    void (*mStartCallback)(void);
    void (*mContinueCallback)(void);
    void (*mStopCallback)(void);
    void (*mActiveSensingCallback)(void);
    void (*mSystemResetCallback)(void);

    // -------------------------------------------------------------------------
    // MIDI Soft Thru

public:
    inline MidiFilterMode getFilterMode() const;
    inline bool getThruState() const;

    inline void turnThruOn(MidiFilterMode inThruFilterMode = Full);
    inline void turnThruOff();
    inline void setThruFilterMode(MidiFilterMode inThruFilterMode);

private:
    void thruFilter(byte inChannel);

private:
    bool parse();
    inline void handleNullVelocityNoteOnAsNoteOff();
    inline bool inputFilter(Channel inChannel);
    inline void resetInput();

private:
    bool            mThruActivated  : 1;
    MidiFilterMode  mThruFilterMode : 7;

private:
    typedef Message<Settings::SysExMaxSize> MidiMessage;

private:
    StatusByte  mRunningStatus_RX;
    StatusByte  mRunningStatus_TX;
    Channel     mInputChannel;
    byte        mPendingMessage[3];
    unsigned    mPendingMessageExpectedLenght;
    unsigned    mPendingMessageIndex;
    MidiMessage mMessage;

private:
    inline StatusByte getStatus(MidiType inType,
                                Channel inChannel) const;

private:
    SerialPort& mSerial;
};

// -----------------------------------------------------------------------------

unsigned encodeSysEx(const byte* inData,  byte* outSysEx, unsigned inLenght);
unsigned decodeSysEx(const byte* inSysEx, byte* outData,  unsigned inLenght);

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

#include "MIDI.hpp"
