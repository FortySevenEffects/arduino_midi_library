/*!
 *  @file       MIDI.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @version    4.0
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#ifndef LIB_MIDI_H_
#define LIB_MIDI_H_

#include <inttypes.h> 
#include "midi_Settings.h"
#include "midi_Defs.h"

BEGIN_MIDI_NAMESPACE

/*! \brief The main class for MIDI handling.
 */
//template<typename Uart>
class MidiInterface
{
public:
    MidiInterface();
    ~MidiInterface();
    
public:
    void begin(byte inChannel = 1);
    
    
    // -------------------------------------------------------------------------
    // MIDI Output
    
#if MIDI_BUILD_OUTPUT
    
public:
    void sendNoteOn(byte inNoteNumber,
                    byte inVelocity,
                    Channel inChannel);
    
    void sendNoteOff(byte inNoteNumber,
                     byte inVelocity,
                     Channel inChannel);
    
    void sendProgramChange(byte inProgramNumber,
                           Channel inChannel);
    
    void sendControlChange(byte inControlNumber,
                           byte inControlValue, 
                           Channel inChannel);
    
    void sendPitchBend(int inPitchValue,    Channel inChannel);
    void sendPitchBend(double inPitchValue, Channel inChannel);
    
    void sendPolyPressure(byte inNoteNumber,
                          byte inPressure,
                          Channel inChannel);
    
    void sendAfterTouch(byte inPressure,
                        Channel inChannel);
    
    void sendSysEx(unsigned int inLength, 
                   const byte* inArray,
                   bool inArrayContainsBoundaries = false);    
    
    void sendTimeCodeQuarterFrame(byte inTypeNibble, 
                                  byte inValuesNibble);
    void sendTimeCodeQuarterFrame(byte inData);
    
    void sendSongPosition(unsigned int inBeats);
    void sendSongSelect(byte inSongNumber);
    void sendTuneRequest();
    void sendRealTime(MidiType inType);
    
    void send(MidiType inType,
              DataByte inData1,
              DataByte inData2,
              Channel inChannel);
    
private:
    inline StatusByte getStatus(MidiType inType,
                                Channel inChannel) const;
    
#endif // MIDI_BUILD_OUTPUT
    
    
    // -------------------------------------------------------------------------
    // MIDI Input
    
#if MIDI_BUILD_INPUT
    
public:
    bool read();
    bool read(Channel inChannel);
    
public:
    inline MidiType getType() const;
    inline Channel  getChannel() const;
    inline DataByte getData1() const;
    inline DataByte getData2() const;
    inline const byte* getSysExArray() const;
    inline unsigned int getSysExArrayLength() const;
    inline bool check() const;
    
public:
    inline Channel getInputChannel() const;
    inline void setInputChannel(Channel inChannel);
    
public:
    static inline MidiType getTypeFromStatusByte(const byte inStatus);
    
private:
    
    bool input_filter(byte inChannel);
    bool parse(byte inChannel);
    void reset_input_attributes();
    
    // Attributes
    byte            mRunningStatus_RX;
    byte            mInputChannel;
    
    byte            mPendingMessage[3];             // SysEx are dumped into mMessage directly.
    unsigned int    mPendingMessageExpectedLenght;
    unsigned int    mPendingMessageIndex;           // Extended to unsigned int for larger SysEx payloads.
    
    midimsg         mMessage;
    
    
    // =========================================================================
    // Input Callbacks
    
#if USE_CALLBACKS
    
public:
    
    void setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity));
    void setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity));
    void setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure));
    void setHandleControlChange(void (*fptr)(byte channel, byte number, byte value));
    void setHandleProgramChange(void (*fptr)(byte channel, byte number));
    void setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure));
    void setHandlePitchBend(void (*fptr)(byte channel, int bend));
    void setHandleSystemExclusive(void (*fptr)(byte * array, byte size));
    void setHandleTimeCodeQuarterFrame(void (*fptr)(byte data));
    void setHandleSongPosition(void (*fptr)(unsigned int beats));
    void setHandleSongSelect(void (*fptr)(byte songnumber));
    void setHandleTuneRequest(void (*fptr)(void));
    void setHandleClock(void (*fptr)(void));
    void setHandleStart(void (*fptr)(void));
    void setHandleContinue(void (*fptr)(void));
    void setHandleStop(void (*fptr)(void));
    void setHandleActiveSensing(void (*fptr)(void));
    void setHandleSystemReset(void (*fptr)(void));
    
    void disconnectCallbackFromType(MidiType Type);
    
private:
    
    void launchCallback();
    
    void (*mNoteOffCallback)(byte channel, byte note, byte velocity);
    void (*mNoteOnCallback)(byte channel, byte note, byte velocity);
    void (*mAfterTouchPolyCallback)(byte channel, byte note, byte velocity);
    void (*mControlChangeCallback)(byte channel, byte, byte);
    void (*mProgramChangeCallback)(byte channel, byte);
    void (*mAfterTouchChannelCallback)(byte channel, byte);
    void (*mPitchBendCallback)(byte channel, int);
    void (*mSystemExclusiveCallback)(byte * array, byte size);
    void (*mTimeCodeQuarterFrameCallback)(byte data);
    void (*mSongPositionCallback)(unsigned int beats);
    void (*mSongSelectCallback)(byte songnumber);
    void (*mTuneRequestCallback)(void);
    void (*mClockCallback)(void);
    void (*mStartCallback)(void);
    void (*mContinueCallback)(void);
    void (*mStopCallback)(void);
    void (*mActiveSensingCallback)(void);
    void (*mSystemResetCallback)(void);
    
#endif // USE_CALLBACKS    
    
#endif // MIDI_BUILD_INPUT
    
    
    // =========================================================================
    // MIDI Soft Thru
    
#if (MIDI_BUILD_INPUT && MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)
    
public:
    inline MidiFilterMode getFilterMode() const;
    inline bool getThruState() const;
    
    inline void turnThruOn(MidiFilterMode inThruFilterMode = Full);
    inline void turnThruOff();
    inline void setThruFilterMode(MidiFilterMode inThruFilterMode);
    
    
private:
    
    void thru_filter(byte inChannel);
    
    bool            mThruActivated  : 1;
    MidiFilterMode  mThruFilterMode : 7;
    
#endif // Thru
    
    
    
    // Attributes
#if MIDI_USE_RUNNING_STATUS
    StatusByte mRunningStatus_TX;
#endif // MIDI_USE_RUNNING_STATUS
    
    
};

END_MIDI_NAMESPACE

#if MIDI_AUTO_INSTANCIATE
extern midi::MidiInterface MIDI;
#endif

#include "midi_Inline.hpp"
