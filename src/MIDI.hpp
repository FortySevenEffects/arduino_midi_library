/*!
 *  @file       MIDI.cpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @version    4.0
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#pragma once

BEGIN_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

template<typename Uart>
MidiInterface<Uart>::MidiInterface()
{
#if MIDI_BUILD_INPUT && MIDI_USE_CALLBACKS
    mNoteOffCallback                = 0;
    mNoteOnCallback                 = 0;
    mAfterTouchPolyCallback         = 0;
    mControlChangeCallback          = 0;
    mProgramChangeCallback          = 0;
    mAfterTouchChannelCallback      = 0;
    mPitchBendCallback              = 0;
    mSystemExclusiveCallback        = 0;
    mTimeCodeQuarterFrameCallback   = 0;
    mSongPositionCallback           = 0;
    mSongSelectCallback             = 0;
    mTuneRequestCallback            = 0;
    mClockCallback                  = 0;
    mStartCallback                  = 0;
    mContinueCallback               = 0;
    mStopCallback                   = 0;
    mActiveSensingCallback          = 0;
    mSystemResetCallback            = 0;
#endif
}

template<typename Uart>
MidiInterface<Uart>::~MidiInterface()
{
}

// -----------------------------------------------------------------------------

#if MIDI_BUILD_OUTPUT

template<typename Uart>
StatusByte MidiInterface<Uart>::getStatus(kMIDIType inType,
                                          Channel inChannel) const
{
    return ((byte)inType | ((inChannel - 1) & 0x0F));
}

#endif // MIDI_BUILD_OUTPUT

// -----------------------------------------------------------------------------

#if MIDI_BUILD_INPUT

/*! \brief Get the last received message's type
 
 Returns an enumerated type. @see MidiType
 */
template<typename Uart>
MidiType MidiInterface<Uart>::getType() const
{
    return mMessage.type;
}

/*! \brief Get the channel of the message stored in the structure.
 
 \return Channel range is 1 to 16. 
 For non-channel messages, this will return 0.
 */
template<typename Uart>
Channel MidiInterface<Uart>::getChannel() const
{
    return mMessage.channel;
}

/*! \brief Get the first data byte of the last received message. */
template<typename Uart>
DataByte MidiInterface<Uart>::getData1() const
{
    return mMessage.data1;
}

/*! \brief Get the second data byte of the last received message. */
template<typename Uart>
DataByte MidiInterface<Uart>::getData2() const
{ 
    return mMessage.data2;
}

/*! \brief Get the System Exclusive byte array. 
 
 @see getSysExArrayLength to get the array's length in bytes.
 */
template<typename Uart>
const byte* MidiInterface<Uart>::getSysExArray() const
{ 
    return mMessage.sysex_array;
}

/*! \brief Get the lenght of the System Exclusive array.
 
 It is coded using data1 as LSB and data2 as MSB.
 \return The array's length, in bytes.
 */
template<typename Uart>
unsigned int MidiInterface<Uart>::getSysExArrayLength() const
{
    const unsigned int size = ((unsigned)(mMessage.data2) << 8) | mMessage.data1;
    return (size > MIDI_SYSEX_ARRAY_SIZE) ? MIDI_SYSEX_ARRAY_SIZE : size;
}

/*! \brief Check if a valid message is stored in the structure. */
template<typename Uart>
bool MidiInterface<Uart>::check() const
{ 
    return mMessage.valid;
}

// -----------------------------------------------------------------------------

template<typename Uart>
Channel MidiInterface<Uart>::getInputChannel() const 
{
    return mInputChannel;
}

/*! \brief Set the value for the input MIDI channel 
 \param Channel the channel value. Valid values are 1 to 16, MIDI_CHANNEL_OMNI 
 if you want to listen to all channels, and MIDI_CHANNEL_OFF to disable input.
 */
template<typename Uart>
void MidiInterface<Uart>::setInputChannel(Channel inChannel)
{ 
    mInputChannel = inChannel;
}

// -----------------------------------------------------------------------------

/*! \brief Extract an enumerated MIDI type from a status byte.
 
 This is a utility static method, used internally, 
 made public so you can handle MidiTypes more easily.
 */
template<typename Uart>
MidiType MidiInterface<Uart>::getTypeFromStatusByte(const byte inStatus) 
{
    if ((inStatus  < 0x80) ||
        (inStatus == 0xF4) ||
        (inStatus == 0xF5) ||
        (inStatus == 0xF9) ||
        (inStatus == 0xFD)) return InvalidType; // data bytes and undefined.
    if (inStatus < 0xF0) return (MidiType)(inStatus & 0xF0);    // Channel message, remove channel nibble.
    else return (MidiType)inStatus;
}

#endif // MIDI_BUILD_INPUT

// -----------------------------------------------------------------------------

#if (MIDI_BUILD_INPUT && MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)

template<typename Uart>
MidiFilterMode MidiInterface<Uart>::getFilterMode() const
{
    return mThruFilterMode;
}

template<typename Uart>
bool MidiInterface<Uart>::getThruState() const
{
    return mThruActivated;
}

/*! \brief Setter method: turn message mirroring on. */
template<typename Uart>
void MidiInterface<Uart>::turnThruOn(MidiFilterMode inThruFilterMode)
{ 
    mThruActivated = true;
    mThruFilterMode = inThruFilterMode;
}


/*! \brief Setter method: turn message mirroring off. */
template<typename Uart>
void MidiInterface<Uart>::turnThruOff()
{
    mThruActivated = false; 
    mThruFilterMode = Off;
}

/*! \brief Set the filter for thru mirroring
 \param inThruFilterMode a filter mode
 
 @see MidiFilterMode
 */
template<typename Uart>
void MidiInterface<Uart>::setThruFilterMode(MidiFilterMode inThruFilterMode)
{ 
    mThruFilterMode = inThruFilterMode;
    if (mThruFilterMode != Off) 
        mThruActivated = true;
    else 
        mThruActivated = false;
}

#endif // MIDI_BUILD_THRU

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
// To sort below these lines.
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


BEGIN_MIDI_NAMESPACE

/*! \brief Default constructor for MidiInterface. */
MidiInterface<Uart>::MidiInterface()
{ 
#if MIDI_BUILD_INPUT && MIDI_USE_CALLBACKS
    
    // Initialise callbacks to 0 pointer
    mNoteOffCallback                = 0;
    mNoteOnCallback                 = 0;
    mAfterTouchPolyCallback         = 0;
    mControlChangeCallback          = 0;
    mProgramChangeCallback          = 0;
    mAfterTouchChannelCallback      = 0;
    mPitchBendCallback              = 0;
    mSystemExclusiveCallback        = 0;
    mTimeCodeQuarterFrameCallback   = 0;
    mSongPositionCallback           = 0;
    mSongSelectCallback             = 0;
    mTuneRequestCallback            = 0;
    mClockCallback                  = 0;
    mStartCallback                  = 0;
    mContinueCallback               = 0;
    mStopCallback                   = 0;
    mActiveSensingCallback          = 0;
    mSystemResetCallback            = 0;
    
#endif
}


/*! \brief Default destructor for MidiInterface.
 
 This is not really useful for the Arduino, as it is never called...
 */
MidiInterface<Uart>::~MidiInterface()
{
    
}


/*! \brief Call the begin method in the setup() function of the Arduino.
 
 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */
template<typename Uart>
void MidiInterface<Uart>::begin(const byte inChannel)
{
    // Initialise the Serial port
    MIDI_SERIAL_PORT.begin(MIDI_BAUDRATE);
    
#if MIDI_BUILD_OUTPUT && MIDI_USE_RUNNING_STATUS
    
    mRunningStatus_TX = InvalidType;
    
#endif // MIDI_BUILD_OUTPUT && MIDI_USE_RUNNING_STATUS
    
    
#if MIDI_BUILD_INPUT
    
    mInputChannel = inChannel;
    mRunningStatus_RX = InvalidType;
    mPendingMessageIndex = 0;
    mPendingMessageExpectedLenght = 0;
    
    mMessage.valid = false;
    mMessage.type = InvalidType;
    mMessage.channel = 0;
    mMessage.data1 = 0;
    mMessage.data2 = 0;
    
#endif // MIDI_BUILD_INPUT
    
    
#if (MIDI_BUILD_INPUT && MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU) // Thru
    
    mThruFilterMode = Full;
    mThruActivated = true;
    
#endif // Thru
    
}


// =============================================================================
// MIDI Output
// =============================================================================

#if MIDI_BUILD_OUTPUT


/*! \brief Generate and send a MIDI message from the values given.
 \param type    The message type (see type defines for reference)
 \param data1   The first data byte.
 \param data2   The second data byte (if the message contains only 1 data byte,
 set this one to 0).
 \param channel The output channel on which the message will be sent 
 (values from 1 to 16). Note: you cannot send to OMNI.
 
 This is an internal method, use it only if you need to send raw data
 from your code, at your own risks.
 */
template<typename Uart>
void MidiInterface<Uart>::send(MidiType type,
                               byte data1,
                               byte data2,
                               byte channel)
{
    // Then test if channel is valid
    if (channel >= MIDI_CHANNEL_OFF  || 
        channel == MIDI_CHANNEL_OMNI || 
        type < NoteOff)
    {
        
#if MIDI_USE_RUNNING_STATUS    
        mRunningStatus_TX = InvalidType;
#endif 
        
        return; // Don't send anything
    }
    
    if (type <= PitchBend)  // Channel messages
    {
        // Protection: remove MSBs on data
        data1 &= 0x7F;
        data2 &= 0x7F;
        
        byte statusbyte = genstatus(type,channel);
        
#if MIDI_USE_RUNNING_STATUS
        // Check Running Status
        if (mRunningStatus_TX != statusbyte)
        {
            // New message, memorise and send header
            mRunningStatus_TX = statusbyte;
            MIDI_SERIAL_PORT.write(mRunningStatus_TX);
        }
#else
        // Don't care about running status, send the Control byte.
        MIDI_SERIAL_PORT.write(statusbyte);
#endif
        
        // Then send data
        MIDI_SERIAL_PORT.write(data1);
        if (type != ProgramChange && type != AfterTouchChannel)
            MIDI_SERIAL_PORT.write(data2);
        
        return;
    }
    if (type >= TuneRequest && type <= SystemReset)
        sendRealTime(type); // System Real-time and 1 byte.
}


/*! \brief Send a Note On message 
 \param NoteNumber  Pitch value in the MIDI format (0 to 127). 
 \param Velocity    Note attack velocity (0 to 127). A
 NoteOn with 0 velocity is considered as a NoteOff.
 \param Channel     The channel on which the message will be sent (1 to 16). 
 
 Take a look at the values, names and frequencies of notes here: 
 http://www.phys.unsw.edu.au/jw/notes.html
 */
template<typename Uart>
void MidiInterface<Uart>::sendNoteOn(byte NoteNumber,
                                     byte Velocity,
                                     byte Channel)
{ 
    send(NoteOn,NoteNumber,Velocity,Channel);
}


/*! \brief Send a Note Off message
 \param NoteNumber  Pitch value in the MIDI format (0 to 127). 
 \param Velocity    Release velocity (0 to 127).
 \param Channel     The channel on which the message will be sent (1 to 16).
 
 Note: you can send NoteOn with zero velocity to make a NoteOff, this is based
 on the Running Status principle, to avoid sending status messages and thus
 sending only NoteOn data. This method will always send a real NoteOff message.
 Take a look at the values, names and frequencies of notes here: 
 http://www.phys.unsw.edu.au/jw/notes.html
 */
template<typename Uart>
void MidiInterface<Uart>::sendNoteOff(byte NoteNumber,
                                      byte Velocity,
                                      byte Channel)
{
    send(NoteOff,NoteNumber,Velocity,Channel);   
}


/*! \brief Send a Program Change message 
 \param ProgramNumber   The Program to select (0 to 127).
 \param Channel         The channel on which the message will be sent (1 to 16).
 */
template<typename Uart>
void MidiInterface<Uart>::sendProgramChange(byte ProgramNumber,
                                            byte Channel)
{
    send(ProgramChange,ProgramNumber,0,Channel);
}


/*! \brief Send a Control Change message 
 \param ControlNumber   The controller number (0 to 127). 
 \param ControlValue    The value for the specified controller (0 to 127).
 \param Channel         The channel on which the message will be sent (1 to 16). 
 
 See the detailed controllers numbers & description here: 
 http://www.somascape.org/midi/tech/spec.html#ctrlnums
 */
template<typename Uart>
void MidiInterface<Uart>::sendControlChange(byte ControlNumber,
                                            byte ControlValue,
                                            byte Channel)
{
    send(ControlChange,ControlNumber,ControlValue,Channel);
}


/*! \brief Send a Polyphonic AfterTouch message (applies to a specified note)
 \param NoteNumber  The note to apply AfterTouch to (0 to 127).
 \param Pressure    The amount of AfterTouch to apply (0 to 127).
 \param Channel     The channel on which the message will be sent (1 to 16). 
 */
template<typename Uart>
void MidiInterface<Uart>::sendPolyPressure(byte NoteNumber,
                                           byte Pressure,
                                           byte Channel)
{
    send(AfterTouchPoly,NoteNumber,Pressure,Channel);
}


/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param Pressure    The amount of AfterTouch to apply to all notes.
 \param Channel     The channel on which the message will be sent (1 to 16). 
 */
template<typename Uart>
void MidiInterface<Uart>::sendAfterTouch(byte Pressure,
                                         byte Channel)
{
    send(AfterTouchChannel,Pressure,0,Channel);
}


/*! \brief Send a Pitch Bend message using a signed integer value.
 \param PitchValue  The amount of bend to send (in a signed integer format), 
 between MIDI_PITCHBEND_MIN and MIDI_PITCHBEND_MAX, 
 center value is 0.
 \param Channel     The channel on which the message will be sent (1 to 16).
 */
template<typename Uart>
void MidiInterface<Uart>::sendPitchBend(int PitchValue,
                                        byte Channel)
{
    const unsigned int bend = PitchValue - MIDI_PITCHBEND_MIN;
    send(PitchBend,(bend & 0x7F),(bend >> 7) & 0x7F,Channel);
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param PitchValue  The amount of bend to send (in a floating point format), 
 between -1.0f (maximum downwards bend) 
 and +1.0f (max upwards bend), center value is 0.0f.
 \param Channel     The channel on which the message will be sent (1 to 16).
 */
template<typename Uart>
void MidiInterface<Uart>::sendPitchBend(double PitchValue,
                                        byte Channel)
{
    const int pitchval = PitchValue * MIDI_PITCHBEND_MAX;
    sendPitchBend(pitchval,Channel);
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
template<typename Uart>
void MidiInterface<Uart>::sendSysEx(int length,
                                    const byte *const array,
                                    bool ArrayContainsBoundaries)
{
    if (ArrayContainsBoundaries == false)
    {
        MIDI_SERIAL_PORT.write(0xF0);
        
        for (int i=0;i<length;++i)
            MIDI_SERIAL_PORT.write(array[i]);
        
        MIDI_SERIAL_PORT.write(0xF7);
    }
    else
    {
        for (int i=0;i<length;++i)
            MIDI_SERIAL_PORT.write(array[i]);
    }
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}


/*! \brief Send a Tune Request message. 
 
 When a MIDI unit receives this message,
 it should tune its oscillators (if equipped with any).
 */
template<typename Uart>
void MidiInterface<Uart>::sendTuneRequest()
{
    sendRealTime(TuneRequest);
}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 \param TypeNibble      MTC type
 \param ValuesNibble    MTC data
 See MIDI Specification for more information.
 */
template<typename Uart>
void MidiInterface<Uart>::sendTimeCodeQuarterFrame(byte TypeNibble, byte ValuesNibble)
{
    const byte data = ( ((TypeNibble & 0x07) << 4) | (ValuesNibble & 0x0F) );
    sendTimeCodeQuarterFrame(data);
}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param data     if you want to encode directly the nibbles in your program, 
 you can send the byte here.
 */
template<typename Uart>
void MidiInterface<Uart>::sendTimeCodeQuarterFrame(byte data)
{
    MIDI_SERIAL_PORT.write((byte)TimeCodeQuarterFrame);
    MIDI_SERIAL_PORT.write(data);
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}


/*! \brief Send a Song Position Pointer message.
 \param Beats    The number of beats since the start of the song.
 */
template<typename Uart>
void MidiInterface<Uart>::sendSongPosition(unsigned int Beats)
{
    MIDI_SERIAL_PORT.write((byte)SongPosition);
    MIDI_SERIAL_PORT.write(Beats & 0x7F);
    MIDI_SERIAL_PORT.write((Beats >> 7) & 0x7F);
    
#if MIDI_USE_RUNNING_STATUS
    mRunningStatus_TX = InvalidType;
#endif
}


/*! \brief Send a Song Select message */
template<typename Uart>
void MidiInterface<Uart>::sendSongSelect(byte SongNumber)
{
    MIDI_SERIAL_PORT.write((byte)SongSelect);
    MIDI_SERIAL_PORT.write(SongNumber & 0x7F);
    
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
template<typename Uart>
void MidiInterface<Uart>::sendRealTime(MidiType Type)
{
    switch (Type) 
    {
        case TuneRequest: // Not really real-time, but one byte anyway.
        case Clock:
        case Start:
        case Stop:    
        case Continue:
        case ActiveSensing:
        case SystemReset:
            MIDI_SERIAL_PORT.write((byte)Type);
            break;
        default:
            // Invalid Real Time marker
            break;
    }
    
    // Do not cancel Running Status for real-time messages as they can be 
    // interleaved within any message. Though, TuneRequest can be sent here, 
    // and as it is a System Common message, it must reset Running Status.
#if MIDI_USE_RUNNING_STATUS
    if (Type == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}

#endif // MIDI_BUILD_OUTPUT


// =============================================================================
// MIDI Input
// =============================================================================

#if MIDI_BUILD_INPUT

/*! \brief Read a MIDI message from the serial port
 using the main input channel (see setInputChannel() for reference).
 
 \return True if a valid message has been stored in the structure, false if not.
 A valid message is a message that matches the input channel. \n\n
 If the Thru is enabled and the messages matches the filter,
 it is sent back on the MIDI output.
 */
template<typename Uart>
bool MidiInterface<Uart>::read()
{
    return read(mInputChannel);
}


/*! \brief Reading/thru-ing method, the same as read()
 with a given input channel to read on. 
 */
template<typename Uart>
bool MidiInterface<Uart>::read(const byte inChannel)
{
    if (inChannel >= MIDI_CHANNEL_OFF)
        return false; // MIDI Input disabled.
    
    if (parse(inChannel))
    {
        if (input_filter(inChannel))
        {
            
#if (MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)
            thru_filter(inChannel);
#endif
            
#if MIDI_USE_CALLBACKS
            launchCallback();
#endif
            return true;
        }
    }
    
    return false;
}


// Private method: MIDI parser
template<typename Uart>
bool MidiInterface<Uart>::parse(byte inChannel)
{ 
    const byte bytes_available = MIDI_SERIAL_PORT.available();
    
    if (bytes_available == 0)
        // No data available.
        return false;
    
    
    /* Parsing algorithm:
     Get a byte from the serial buffer.
     * If there is no pending message to be recomposed, start a new one.
     - Find type and channel (if pertinent)
     - Look for other bytes in buffer, call parser recursively, 
     until the message is assembled or the buffer is empty.
     * Else, add the extracted byte to the pending message, and check validity. 
     When the message is done, store it.
     */
    
    const byte extracted = MIDI_SERIAL_PORT.read();
    
    if (mPendingMessageIndex == 0) 
    { 
        // Start a new pending message
        mPendingMessage[0] = extracted;
        
        // Check for running status first
        switch (getTypeFromStatusByte(mRunningStatus_RX))
        {
                // Only these types allow Running Status:
            case NoteOff:
            case NoteOn:
            case AfterTouchPoly:
            case ControlChange:
            case ProgramChange:
            case AfterTouchChannel:
            case PitchBend:
                
                // If the status byte is not received, prepend it 
                // to the pending message
                if (extracted < 0x80)
                {
                    mPendingMessage[0] = mRunningStatus_RX;
                    mPendingMessage[1] = extracted;
                    mPendingMessageIndex = 1;
                }
                // Else: well, we received another status byte,
                // so the running status does not apply here.
                // It will be updated upon completion of this message.
                
                if (mPendingMessageIndex >= (mPendingMessageExpectedLenght-1))
                {
                    mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
                    mMessage.channel = (mPendingMessage[0] & 0x0F)+1;
                    mMessage.data1 = mPendingMessage[1];
                    
                    // Save data2 only if applicable
                    if (mPendingMessageExpectedLenght == 3)
                        mMessage.data2 = mPendingMessage[2];
                    else 
                        mMessage.data2 = 0;
                    
                    mPendingMessageIndex = 0;
                    mPendingMessageExpectedLenght = 0;
                    mMessage.valid = true;
                    return true;
                }
                break;
            default:
                // No running status
                break;
        }
        
        
        switch (getTypeFromStatusByte(mPendingMessage[0]))
        {
                // 1 byte messages
            case Start:
            case Continue:
            case Stop:
            case Clock:
            case ActiveSensing:
            case SystemReset:
            case TuneRequest:
                // Handle the message type directly here.
                mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
                mMessage.channel = 0;
                mMessage.data1 = 0;
                mMessage.data2 = 0;
                mMessage.valid = true;
                
                // \fix Running Status broken when receiving Clock messages.
                // Do not reset all input attributes, Running Status must remain unchanged.
                //reset_input_attributes(); 
                
                // We still need to reset these
                mPendingMessageIndex = 0;
                mPendingMessageExpectedLenght = 0;
                
                return true;
                break;
                
                // 2 bytes messages
            case ProgramChange:
            case AfterTouchChannel:
            case TimeCodeQuarterFrame:
            case SongSelect:
                mPendingMessageExpectedLenght = 2;
                break;
                
                // 3 bytes messages
            case NoteOn:
            case NoteOff:
            case ControlChange:
            case PitchBend:
            case AfterTouchPoly:
            case SongPosition:
                mPendingMessageExpectedLenght = 3;
                break;
                
            case SystemExclusive:
                // The message can be any lenght 
                // between 3 and MIDI_SYSEX_ARRAY_SIZE bytes
                mPendingMessageExpectedLenght = MIDI_SYSEX_ARRAY_SIZE; 
                mRunningStatus_RX = InvalidType;
                break;
                
            case InvalidType:
            default:
                // This is obviously wrong. Let's get the hell out'a here.
                reset_input_attributes();
                return false;
                break;
        }
        
        // Then update the index of the pending message.
        mPendingMessageIndex++;
        
#if USE_1BYTE_PARSING
        // Message is not complete.
        return false;
#else
        // Call the parser recursively
        // to parse the rest of the message.
        return parse(inChannel);
#endif
        
    }
    else
    { 
        // First, test if this is a status byte
        if (extracted >= 0x80)
        {
            // Reception of status bytes in the middle of an uncompleted message
            // are allowed only for interleaved Real Time message or EOX
            switch (extracted)
            {
                case Clock:
                case Start:
                case Continue:
                case Stop:
                case ActiveSensing:
                case SystemReset:
                    
                    // Here we will have to extract the one-byte message, 
                    // pass it to the structure for being read outside
                    // the MIDI class, and recompose the message it was 
                    // interleaved into. Oh, and without killing the running status.. 
                    // This is done by leaving the pending message as is, 
                    // it will be completed on next calls.
                    
                    mMessage.type = (MidiType)extracted;
                    mMessage.data1 = 0;
                    mMessage.data2 = 0;
                    mMessage.channel = 0;
                    mMessage.valid = true;
                    return true;
                    
                    break;
                    
                    // End of Exclusive
                case 0xF7:
                    if (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive)
                    {
                        // Store System Exclusive array in midimsg structure
                        for (byte i=0;i<MIDI_SYSEX_ARRAY_SIZE;i++)
                            mMessage.sysex_array[i] = mPendingMessage[i];
                        
                        mMessage.type = SystemExclusive;
                        
                        // Get length
                        mMessage.data1 = (mPendingMessageIndex+1) & 0xFF;    
                        mMessage.data2 = (mPendingMessageIndex+1) >> 8;
                        mMessage.channel = 0;
                        mMessage.valid = true;
                        
                        reset_input_attributes();
                        return true;
                    }
                    else
                    {
                        // Well well well.. error.
                        reset_input_attributes();
                        return false;
                    }
                    
                    break;
                default:
                    break;
            }
        }
        
        // Add extracted data byte to pending message
        mPendingMessage[mPendingMessageIndex] = extracted;
        
        // Now we are going to check if we have reached the end of the message
        if (mPendingMessageIndex >= (mPendingMessageExpectedLenght-1))
        {
            // "FML" case: fall down here with an overflown SysEx..
            // This means we received the last possible data byte that can fit 
            // the buffer. If this happens, try increasing MIDI_SYSEX_ARRAY_SIZE.
            if (getTypeFromStatusByte(mPendingMessage[0]) == SystemExclusive)
            {
                reset_input_attributes();
                return false;
            }
            
            mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);
            // Don't check if it is a Channel Message
            mMessage.channel = (mPendingMessage[0] & 0x0F)+1;
            
            mMessage.data1 = mPendingMessage[1];
            
            // Save data2 only if applicable
            if (mPendingMessageExpectedLenght == 3)
                mMessage.data2 = mPendingMessage[2];
            else 
                mMessage.data2 = 0;
            
            // Reset local variables
            mPendingMessageIndex = 0;
            mPendingMessageExpectedLenght = 0;
            
            mMessage.valid = true;
            
            // Activate running status (if enabled for the received type)
            switch (mMessage.type)
            {
                case NoteOff:
                case NoteOn:
                case AfterTouchPoly:
                case ControlChange:
                case ProgramChange:
                case AfterTouchChannel:
                case PitchBend:    
                    // Running status enabled: store it from received message
                    mRunningStatus_RX = mPendingMessage[0];
                    break;
                    
                default:
                    // No running status
                    mRunningStatus_RX = InvalidType;
                    break;
            }
            return true;
        }
        else
        {
            // Then update the index of the pending message.
            mPendingMessageIndex++;
            
#if USE_1BYTE_PARSING
            // Message is not complete.
            return false;
#else
            // Call the parser recursively
            // to parse the rest of the message.
            return parse(inChannel);
#endif
        }
    }
    
    // What are our chances to fall here?
    return false;
}


// Private method: check if the received message is on the listened channel
template<typename Uart>
bool MidiInterface<Uart>::input_filter(byte inChannel)
{
    // This method handles recognition of channel 
    // (to know if the message is destinated to the Arduino)
    
    if (mMessage.type == InvalidType)
        return false;
    
    // First, check if the received message is Channel
    if (mMessage.type >= NoteOff && mMessage.type <= PitchBend)
    {
        // Then we need to know if we listen to it
        if ((mMessage.channel == mInputChannel) || 
            (mInputChannel == MIDI_CHANNEL_OMNI))
        {
            return true;
        }
        else 
        {
            // We don't listen to this channel
            return false;
        }
    }
    else 
    {
        // System messages are always received
        return true;
    }
}


// Private method: reset input attributes
template<typename Uart>
void MidiInterface<Uart>::reset_input_attributes()
{
    mPendingMessageIndex = 0;
    mPendingMessageExpectedLenght = 0;
    mRunningStatus_RX = InvalidType;
}






#if MIDI_USE_CALLBACKS

void MidiInterface<Uart>::setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity))         { mNoteOffCallback = fptr; }
void MidiInterface<Uart>::setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity))          { mNoteOnCallback = fptr; }
void MidiInterface<Uart>::setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure))  { mAfterTouchPolyCallback = fptr; }
void MidiInterface<Uart>::setHandleControlChange(void (*fptr)(byte channel, byte number, byte value))    { mControlChangeCallback = fptr; }
void MidiInterface<Uart>::setHandleProgramChange(void (*fptr)(byte channel, byte number))                { mProgramChangeCallback = fptr; }
void MidiInterface<Uart>::setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure))          { mAfterTouchChannelCallback = fptr; }
void MidiInterface<Uart>::setHandlePitchBend(void (*fptr)(byte channel, int bend))                       { mPitchBendCallback = fptr; }
void MidiInterface<Uart>::setHandleSystemExclusive(void (*fptr)(byte * array, byte size))                { mSystemExclusiveCallback = fptr; }
void MidiInterface<Uart>::setHandleTimeCodeQuarterFrame(void (*fptr)(byte data))                         { mTimeCodeQuarterFrameCallback = fptr; }
void MidiInterface<Uart>::setHandleSongPosition(void (*fptr)(unsigned int beats))                        { mSongPositionCallback = fptr; }
void MidiInterface<Uart>::setHandleSongSelect(void (*fptr)(byte songnumber))                             { mSongSelectCallback = fptr; }
void MidiInterface<Uart>::setHandleTuneRequest(void (*fptr)(void))                                       { mTuneRequestCallback = fptr; }
void MidiInterface<Uart>::setHandleClock(void (*fptr)(void))                                             { mClockCallback = fptr; }
void MidiInterface<Uart>::setHandleStart(void (*fptr)(void))                                             { mStartCallback = fptr; }
void MidiInterface<Uart>::setHandleContinue(void (*fptr)(void))                                          { mContinueCallback = fptr; }
void MidiInterface<Uart>::setHandleStop(void (*fptr)(void))                                              { mStopCallback = fptr; }
void MidiInterface<Uart>::setHandleActiveSensing(void (*fptr)(void))                                     { mActiveSensingCallback = fptr; }
void MidiInterface<Uart>::setHandleSystemReset(void (*fptr)(void))                                       { mSystemResetCallback = fptr; }


/*! \brief Detach an external function from the given type.
 
 Use this method to cancel the effects of setHandle********.
 \param Type        The type of message to unbind. 
 When a message of this type is received, no function will be called.
 */
template<typename Uart>
void MidiInterface<Uart>::disconnectCallbackFromType(MidiType Type)
{
    switch (Type)
    {
        case NoteOff:               mNoteOffCallback = 0;                break;
        case NoteOn:                mNoteOnCallback = 0;                 break;
        case AfterTouchPoly:        mAfterTouchPolyCallback = 0;         break;
        case ControlChange:         mControlChangeCallback = 0;          break;
        case ProgramChange:         mProgramChangeCallback = 0;          break;
        case AfterTouchChannel:     mAfterTouchChannelCallback = 0;      break;
        case PitchBend:             mPitchBendCallback = 0;              break;
        case SystemExclusive:       mSystemExclusiveCallback = 0;        break;
        case TimeCodeQuarterFrame:  mTimeCodeQuarterFrameCallback = 0;   break;
        case SongPosition:          mSongPositionCallback = 0;           break;
        case SongSelect:            mSongSelectCallback = 0;             break;
        case TuneRequest:           mTuneRequestCallback = 0;            break;
        case Clock:                 mClockCallback = 0;                  break;
        case Start:                 mStartCallback = 0;                  break;
        case Continue:              mContinueCallback = 0;               break;
        case Stop:                  mStopCallback = 0;                   break;
        case ActiveSensing:         mActiveSensingCallback = 0;          break;
        case SystemReset:           mSystemResetCallback = 0;            break;
        default:
            break;
    }
}


// Private - launch callback function based on received type.
template<typename Uart>
void MidiInterface<Uart>::launchCallback()
{
    // The order is mixed to allow frequent messages to trigger their callback faster.
    switch (mMessage.type)
    {
            // Notes
        case NoteOff:               if (mNoteOffCallback != 0)               mNoteOffCallback(mMessage.channel,mMessage.data1,mMessage.data2);   break;
        case NoteOn:                if (mNoteOnCallback != 0)                mNoteOnCallback(mMessage.channel,mMessage.data1,mMessage.data2);    break;
            
            // Real-time messages
        case Clock:                 if (mClockCallback != 0)                 mClockCallback();           break;            
        case Start:                 if (mStartCallback != 0)                 mStartCallback();           break;
        case Continue:              if (mContinueCallback != 0)              mContinueCallback();        break;
        case Stop:                  if (mStopCallback != 0)                  mStopCallback();            break;
        case ActiveSensing:         if (mActiveSensingCallback != 0)         mActiveSensingCallback();   break;
            
            // Continuous controllers
        case ControlChange:         if (mControlChangeCallback != 0)         mControlChangeCallback(mMessage.channel,mMessage.data1,mMessage.data2);    break;
        case PitchBend:             if (mPitchBendCallback != 0)             mPitchBendCallback(mMessage.channel,(int)((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7)) + MIDI_PITCHBEND_MIN); break; // TODO: check this
        case AfterTouchPoly:        if (mAfterTouchPolyCallback != 0)        mAfterTouchPolyCallback(mMessage.channel,mMessage.data1,mMessage.data2);    break;
        case AfterTouchChannel:     if (mAfterTouchChannelCallback != 0)     mAfterTouchChannelCallback(mMessage.channel,mMessage.data1);    break;
            
        case ProgramChange:         if (mProgramChangeCallback != 0)         mProgramChangeCallback(mMessage.channel,mMessage.data1);    break;
        case SystemExclusive:       if (mSystemExclusiveCallback != 0)       mSystemExclusiveCallback(mMessage.sysex_array,mMessage.data1);    break;
            
            // Occasional messages
        case TimeCodeQuarterFrame:  if (mTimeCodeQuarterFrameCallback != 0)  mTimeCodeQuarterFrameCallback(mMessage.data1);    break;
        case SongPosition:          if (mSongPositionCallback != 0)          mSongPositionCallback((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7));    break;
        case SongSelect:            if (mSongSelectCallback != 0)            mSongSelectCallback(mMessage.data1);    break;
        case TuneRequest:           if (mTuneRequestCallback != 0)           mTuneRequestCallback();    break;
            
        case SystemReset:           if (mSystemResetCallback != 0)           mSystemResetCallback();    break;
        case InvalidType:
        default:
            break;
    }
}


#endif // MIDI_USE_CALLBACKS


#endif // MIDI_BUILD_INPUT


// =============================================================================
// MIDI Soft Thru
// =============================================================================

#if (MIDI_BUILD_INPUT && MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)

// This method is called upon reception of a message 
// and takes care of Thru filtering and sending.
template<typename Uart>
void MidiInterface<Uart>::thru_filter(byte inChannel)
{
    
    /*
     This method handles Soft-Thru filtering.
     
     Soft-Thru filtering:
     - All system messages (System Exclusive, Common and Real Time) are passed to output unless filter is set to Off
     - Channel messages are passed to the output whether their channel is matching the input channel and the filter setting
     
     */
    
    // If the feature is disabled, don't do anything.
    if (!mThruActivated || (mThruFilterMode == Off))
        return;
    
    
    // First, check if the received message is Channel
    if (mMessage.type >= NoteOff && mMessage.type <= PitchBend)
    {
        const bool filter_condition = ((mMessage.channel == mInputChannel) || 
                                       (mInputChannel == MIDI_CHANNEL_OMNI));
        
        // Now let's pass it to the output
        switch (mThruFilterMode)
        {
            case Full:
                send(mMessage.type,
                     mMessage.data1,
                     mMessage.data2,
                     mMessage.channel);
                return;
                break;
            case SameChannel:
                if (filter_condition)
                {
                    send(mMessage.type,
                         mMessage.data1,
                         mMessage.data2,
                         mMessage.channel);
                    return;
                }
                break;
            case DifferentChannel:
                if (!filter_condition)
                {
                    send(mMessage.type,
                         mMessage.data1,
                         mMessage.data2,
                         mMessage.channel);
                    return;
                }
                break;
            case Off:
                // Do nothing. 
                // Technically it's impossible to get there because 
                // the case was already tested earlier.
                break;
            default:
                break;
        }
    }
    else
    {
        // Send the message to the output
        switch (mMessage.type)
        {
                // Real Time and 1 byte
            case Clock:
            case Start:
            case Stop:
            case Continue:
            case ActiveSensing:
            case SystemReset:
            case TuneRequest:    
                sendRealTime(mMessage.type);
                return;
                break;
                
            case SystemExclusive:
                // Send SysEx (0xF0 and 0xF7 are included in the buffer)
                sendSysEx(mMessage.data1,mMessage.sysex_array,true); 
                return;
                break;
                
            case SongSelect:
                sendSongSelect(mMessage.data1);
                return;
                break;
                
            case SongPosition:
                sendSongPosition(mMessage.data1 | ((unsigned)mMessage.data2<<7));
                return;
                break;
                
            case TimeCodeQuarterFrame:
                sendTimeCodeQuarterFrame(mMessage.data1,mMessage.data2);
                return;
                break;
            default:
                break;
        }
    }
}

#endif // Thru

END_MIDI_NAMESPACE
