/*!
 *  @file       midi_Inline.hpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Inline implementations
 *  @version    4.0
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#pragma once

BEGIN_MIDI_NAMESPACE

// -----------------------------------------------------------------------------
//                                 Output
// -----------------------------------------------------------------------------

#if MIDI_BUILD_OUTPUT

/*! \brief Send a Note On message 
 \param inNoteNumber  Pitch value in the MIDI format (0 to 127). 
 \param inVelocity    Note attack velocity (0 to 127). A NoteOn with 0 velocity 
 is considered as a NoteOff.
 \param inChannel     The channel on which the message will be sent (1 to 16). 
 
 Take a look at the values, names and frequencies of notes here: 
 http://www.phys.unsw.edu.au/jw/notes.html
 */
void MidiInterface::sendNoteOn(DataByte inNoteNumber,
                               DataByte inVelocity,
                               Channel inChannel)
{ 
    send(NoteOn, inNoteNumber, inVelocity, inChannel);
}

/*! \brief Send a Note Off message
 \param inNoteNumber  Pitch value in the MIDI format (0 to 127). 
 \param inVelocity    Release velocity (0 to 127).
 \param inChannel     The channel on which the message will be sent (1 to 16).
 
 Note: you can send NoteOn with zero velocity to make a NoteOff, this is based
 on the Running Status principle, to avoid sending status messages and thus
 sending only NoteOn data. This method will always send a real NoteOff message.
 Take a look at the values, names and frequencies of notes here: 
 http://www.phys.unsw.edu.au/jw/notes.html
 */
void MidiInterface::sendNoteOff(DataByte inNoteNumber,
                                DataByte inVelocity,
                                Channel inChannel)
{
    send(NoteOff, inNoteNumber, inVelocity, inChannel);
}

/*! \brief Send a Program Change message 
 \param inProgramNumber The Program to select (0 to 127).
 \param inChannel       The channel on which the message will be sent (1 to 16).
 */
void MidiInterface::sendProgramChange(DataByte inProgramNumber,
                                      Channel inChannel)
{
    send(ProgramChange, inProgramNumber, 0, inChannel);
}

/*! \brief Send a Control Change message 
 \param ControlNumber   The controller number (0 to 127). 
 \param ControlValue    The value for the specified controller (0 to 127).
 \param Channel         The channel on which the message will be sent (1 to 16). 
 
 See the detailed controllers numbers & description here: 
 http://www.somascape.org/midi/tech/spec.html#ctrlnums
 */
void MidiInterface::sendControlChange(DataByte inControlNumber,
                                      DataByte inControlValue,
                                      Channel inChannel)
{
    send(ControlChange, inControlNumber, inControlValue, inChannel);
}

/*! \brief Send a Polyphonic AfterTouch message (applies to a specified note)
 \param NoteNumber  The note to apply AfterTouch to (0 to 127).
 \param Pressure    The amount of AfterTouch to apply (0 to 127).
 \param Channel     The channel on which the message will be sent (1 to 16). 
 */
void MidiInterface::sendPolyPressure(DataByte inNoteNumber,
                                     DataByte inPressure,
                                     Channel inChannel)
{
    send(AfterTouchPoly, inNoteNumber, inPressure, inChannel);
}

/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param Pressure    The amount of AfterTouch to apply to all notes.
 \param Channel     The channel on which the message will be sent (1 to 16). 
 */
void MidiInterface::sendAfterTouch(DataByte inPressure,
                                   Channel inChannel)
{
    send(AfterTouchChannel, inPressure, 0, inChannel);
}

/*! \brief Send a Pitch Bend message using a signed integer value.
 \param PitchValue  The amount of bend to send (in a signed integer format), 
 between MIDI_PITCHBEND_MIN and MIDI_PITCHBEND_MAX, 
 center value is 0.
 \param Channel     The channel on which the message will be sent (1 to 16).
 */
void MidiInterface::sendPitchBend(int inPitchValue,
                                  Channel inChannel)
{
    const unsigned int bend = inPitchValue - MIDI_PITCHBEND_MIN;
    send(PitchBend, (bend & 0x7F), (bend >> 7) & 0x7F, inChannel);
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param PitchValue  The amount of bend to send (in a floating point format), 
 between -1.0f (maximum downwards bend) 
 and +1.0f (max upwards bend), center value is 0.0f.
 \param Channel     The channel on which the message will be sent (1 to 16).
 */
void MidiInterface::sendPitchBend(double inPitchValue,
                                  Channel inChannel)
{
    const int value = inPitchValue * MIDI_PITCHBEND_MAX;
    sendPitchBend(value, inChannel);
}

/*! \brief Generate and send a System Exclusive frame.
 \param length  The size of the array to send
 \param array   The byte array containing the data to send
 \param ArrayContainsBoundaries When set to 'true', 0xF0 & 0xF7 bytes
 (start & stop SysEx) will NOT be sent
 (and therefore must be included in the array).
 default value for ArrayContainsBoundaries is set to 'false' for compatibility
 with previous versions of the library.
 */
void MidiInterface::sendSysEx(unsigned int inLength,
                              const byte* inArray,
                              bool inArrayContainsBoundaries)
{
    if (inArrayContainsBoundaries == false)
    {
        MIDI_SERIAL_PORT.write(0xF0);
        
        for (unsigned int i=0;i<inLength;++i)
            MIDI_SERIAL_PORT.write(inArray[i]);
        
        MIDI_SERIAL_PORT.write(0xF7);
    }
    else
    {
        for (unsigned int i=0;i<inLength;++i)
            MIDI_SERIAL_PORT.write(inArray[i]);
    }
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}

/*! \brief Send a Tune Request message. 
 
 When a MIDI unit receives this message,
 it should tune its oscillators (if equipped with any).
 */
void MidiInterface::sendTuneRequest()
{
    sendRealTime(TuneRequest);
}

/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 \param TypeNibble      MTC type
 \param ValuesNibble    MTC data
 See MIDI Specification for more information.
 */
void MidiInterface::sendTimeCodeQuarterFrame(DataByte inTypeNibble, 
                                             DataByte inValuesNibble)
{
    const byte data = ( ((inTypeNibble & 0x07) << 4) | (inValuesNibble & 0x0F) );
    sendTimeCodeQuarterFrame(data);
}

/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param data     if you want to encode directly the nibbles in your program, 
 you can send the byte here.
 */
void MidiInterface::sendTimeCodeQuarterFrame(DataByte inData)
{
    MIDI_SERIAL_PORT.write((byte)TimeCodeQuarterFrame);
    MIDI_SERIAL_PORT.write(inData);
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}

/*! \brief Send a Song Position Pointer message.
 \param Beats    The number of beats since the start of the song.
 */
void MidiInterface::sendSongPosition(unsigned int inBeats)
{
    MIDI_SERIAL_PORT.write((byte)SongPosition);
    MIDI_SERIAL_PORT.write(inBeats & 0x7F);
    MIDI_SERIAL_PORT.write((inBeats >> 7) & 0x7F);
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}

/*! \brief Send a Song Select message */
void MidiInterface::sendSongSelect(DataByte inSongNumber)
{
    MIDI_SERIAL_PORT.write((byte)SongSelect);
    MIDI_SERIAL_PORT.write(inSongNumber & 0x7F);
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}

/*! \brief Send a Real Time (one byte) message. 
 
 \param Type    The available Real Time types are: 
 Start, Stop, Continue, Clock, ActiveSensing and SystemReset.
 You can also send a Tune Request with this method.
 @see MidiType
 */
void MidiInterface::sendRealTime(MidiType inType)
{
    switch (inType) 
    {
        case TuneRequest: // Not really real-time, but one byte anyway.
        case Clock:
        case Start:
        case Stop:    
        case Continue:
        case ActiveSensing:
        case SystemReset:
            MIDI_SERIAL_PORT.write((byte)inType);
            break;
        default:
            // Invalid Real Time marker
            break;
    }
    
    // Do not cancel Running Status for real-time messages as they can be 
    // interleaved within any message. Though, TuneRequest can be sent here, 
    // and as it is a System Common message, it must reset Running Status.
#if MIDI_USE_RUNNING_STATUS
    if (inType == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}

// -----------------------------------------------------------------------------

StatusByte MidiInterface::getStatus(MidiType inType,
                                    Channel inChannel) const
{
    return ((byte)inType | ((inChannel - 1) & 0x0F));
}

#endif // MIDI_BUILD_OUTPUT


// -----------------------------------------------------------------------------
//                                  Input
// -----------------------------------------------------------------------------

#if MIDI_BUILD_INPUT

/*! \brief Get the last received message's type
 
 Returns an enumerated type. @see MidiType
 */
MidiType MidiInterface::getType() const
{
    return mMessage.type;
}

/*! \brief Get the channel of the message stored in the structure.
 
 \return Channel range is 1 to 16. 
 For non-channel messages, this will return 0.
 */
Channel MidiInterface::getChannel() const
{
    return mMessage.channel;
}

/*! \brief Get the first data byte of the last received message. */
DataByte MidiInterface::getData1() const
{
    return mMessage.data1;
}

/*! \brief Get the second data byte of the last received message. */
DataByte MidiInterface::getData2() const
{ 
    return mMessage.data2;
}

/*! \brief Get the System Exclusive byte array. 
 
 @see getSysExArrayLength to get the array's length in bytes.
 */
const byte* MidiInterface::getSysExArray() const
{ 
    return mMessage.sysex_array;
}

/*! \brief Get the lenght of the System Exclusive array.
 
 It is coded using data1 as LSB and data2 as MSB.
 \return The array's length, in bytes.
 */
unsigned int MidiInterface::getSysExArrayLength() const
{
    const unsigned int size = ((unsigned)(mMessage.data2) << 8) | mMessage.data1;
    return (size > MIDI_SYSEX_ARRAY_SIZE) ? MIDI_SYSEX_ARRAY_SIZE : size;
}

/*! \brief Check if a valid message is stored in the structure. */
bool MidiInterface::check() const
{ 
    return mMessage.valid;
}

// -----------------------------------------------------------------------------

Channel MidiInterface::getInputChannel() const 
{
    return mInputChannel;
}

/*! \brief Set the value for the input MIDI channel 
 \param Channel the channel value. Valid values are 1 to 16, MIDI_CHANNEL_OMNI 
 if you want to listen to all channels, and MIDI_CHANNEL_OFF to disable input.
 */
void MidiInterface::setInputChannel(Channel inChannel)
{ 
    mInputChannel = inChannel;
}

// -----------------------------------------------------------------------------

/*! \brief Extract an enumerated MIDI type from a status byte.
 
 This is a utility static method, used internally, 
 made public so you can handle MidiTypes more easily.
 */
MidiType MidiInterface::getTypeFromStatusByte(const byte inStatus) 
{
    if ((inStatus  < 0x80) ||
        (inStatus == 0xF4) ||
        (inStatus == 0xF5) ||
        (inStatus == 0xF9) ||
        (inStatus == 0xFD)) return InvalidType; // data bytes and undefined.
    if (inStatus < 0xF0) return (MidiType)(inStatus & 0xF0);    // Channel message, remove channel nibble.
    else return (MidiType)inStatus;
}

// -----------------------------------------------------------------------------

#if MIDI_USE_CALLBACKS

void MidiInterface::setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity))          { mNoteOffCallback              = fptr; }
void MidiInterface::setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity))           { mNoteOnCallback               = fptr; }
void MidiInterface::setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure))   { mAfterTouchPolyCallback       = fptr; }
void MidiInterface::setHandleControlChange(void (*fptr)(byte channel, byte number, byte value))     { mControlChangeCallback        = fptr; }
void MidiInterface::setHandleProgramChange(void (*fptr)(byte channel, byte number))                 { mProgramChangeCallback        = fptr; }
void MidiInterface::setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure))           { mAfterTouchChannelCallback    = fptr; }
void MidiInterface::setHandlePitchBend(void (*fptr)(byte channel, int bend))                        { mPitchBendCallback            = fptr; }
void MidiInterface::setHandleSystemExclusive(void (*fptr)(byte* array, byte size))                  { mSystemExclusiveCallback      = fptr; }
void MidiInterface::setHandleTimeCodeQuarterFrame(void (*fptr)(byte data))                          { mTimeCodeQuarterFrameCallback = fptr; }
void MidiInterface::setHandleSongPosition(void (*fptr)(unsigned int beats))                         { mSongPositionCallback         = fptr; }
void MidiInterface::setHandleSongSelect(void (*fptr)(byte songnumber))                              { mSongSelectCallback           = fptr; }
void MidiInterface::setHandleTuneRequest(void (*fptr)(void))                                        { mTuneRequestCallback          = fptr; }
void MidiInterface::setHandleClock(void (*fptr)(void))                                              { mClockCallback                = fptr; }
void MidiInterface::setHandleStart(void (*fptr)(void))                                              { mStartCallback                = fptr; }
void MidiInterface::setHandleContinue(void (*fptr)(void))                                           { mContinueCallback             = fptr; }
void MidiInterface::setHandleStop(void (*fptr)(void))                                               { mStopCallback                 = fptr; }
void MidiInterface::setHandleActiveSensing(void (*fptr)(void))                                      { mActiveSensingCallback        = fptr; }
void MidiInterface::setHandleSystemReset(void (*fptr)(void))                                        { mSystemResetCallback          = fptr; }

/*! \brief Detach an external function from the given type.
 
 Use this method to cancel the effects of setHandle********.
 \param Type        The type of message to unbind. 
 When a message of this type is received, no function will be called.
 */
void MidiInterface::disconnectCallbackFromType(MidiType inType)
{
    switch (inType)
    {
        case NoteOff:               mNoteOffCallback                = 0; break;
        case NoteOn:                mNoteOnCallback                 = 0; break;
        case AfterTouchPoly:        mAfterTouchPolyCallback         = 0; break;
        case ControlChange:         mControlChangeCallback          = 0; break;
        case ProgramChange:         mProgramChangeCallback          = 0; break;
        case AfterTouchChannel:     mAfterTouchChannelCallback      = 0; break;
        case PitchBend:             mPitchBendCallback              = 0; break;
        case SystemExclusive:       mSystemExclusiveCallback        = 0; break;
        case TimeCodeQuarterFrame:  mTimeCodeQuarterFrameCallback   = 0; break;
        case SongPosition:          mSongPositionCallback           = 0; break;
        case SongSelect:            mSongSelectCallback             = 0; break;
        case TuneRequest:           mTuneRequestCallback            = 0; break;
        case Clock:                 mClockCallback                  = 0; break;
        case Start:                 mStartCallback                  = 0; break;
        case Continue:              mContinueCallback               = 0; break;
        case Stop:                  mStopCallback                   = 0; break;
        case ActiveSensing:         mActiveSensingCallback          = 0; break;
        case SystemReset:           mSystemResetCallback            = 0; break;
        default:
            break;
    }
}

#endif // MIDI_USE_CALLBACKS

#endif // MIDI_BUILD_INPUT


// -----------------------------------------------------------------------------
//                                  Thru
// -----------------------------------------------------------------------------

#if (MIDI_BUILD_INPUT && MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)

MidiFilterMode MidiInterface::getFilterMode() const
{
    return mThruFilterMode;
}

bool MidiInterface::getThruState() const
{
    return mThruActivated;
}

/*! \brief Setter method: turn message mirroring on. */
void MidiInterface::turnThruOn(MidiFilterMode inThruFilterMode)
{ 
    mThruActivated = true;
    mThruFilterMode = inThruFilterMode;
}


/*! \brief Setter method: turn message mirroring off. */
void MidiInterface::turnThruOff()
{
    mThruActivated = false; 
    mThruFilterMode = Off;
}

/*! \brief Set the filter for thru mirroring
 \param inThruFilterMode a filter mode
 
 @see MidiFilterMode
 */
void MidiInterface::setThruFilterMode(MidiFilterMode inThruFilterMode)
{ 
    mThruFilterMode = inThruFilterMode;
    if (mThruFilterMode != Off) 
        mThruActivated = true;
    else 
        mThruActivated = false;
}

#endif // MIDI_BUILD_THRU

// -----------------------------------------------------------------------------

END_MIDI_NAMESPACE
