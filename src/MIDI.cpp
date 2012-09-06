/*!
 *  @file       MIDI.cpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino
 *  @version    4.0
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#include "MIDI.h"

// -----------------------------------------------------------------------------

#if !(MIDI_BUILD_INPUT) && !(MIDI_BUILD_OUTPUT)
#   error To use MIDI, you need to enable at least input or output.
#endif

#if MIDI_BUILD_THRU && !(MIDI_BUILD_OUTPUT)
#   error For thru to work, you need to enable output.
#endif
#if MIDI_BUILD_THRU && !(MIDI_BUILD_INPUT)
#   error For thru to work, you need to enable input.
#endif

// -----------------------------------------------------------------------------

#if MIDI_AUTO_INSTANCIATE
#   if MIDI_USE_SOFTWARE_SERIAL
#       include "../SoftwareSerial/SoftwareSerial.h"
        SoftwareSerial softSerialClass(MIDI_SOFTSERIAL_RX_PIN,
                                       MIDI_SOFTSERIAL_TX_PIN);
#       undef  MIDI_SERIAL_PORT
#       define MIDI_SERIAL_PORT softSerialClass
#   else
#       include "HardwareSerial.h"
#   endif // MIDI_USE_SOFTWARE_SERIAL

    MIDI_NAMESPACE::MidiInterface MIDI;

#endif // MIDI_AUTO_INSTANCIATE

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

/*! \brief Constructor for MidiInterface. */
MidiInterface::MidiInterface()
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

/*! \brief Destructor for MidiInterface.
 
 This is not really useful for the Arduino, as it is never called...
 */
MidiInterface::~MidiInterface()
{
}

// -----------------------------------------------------------------------------

/*! \brief Call the begin method in the setup() function of the Arduino.
 
 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */
void MidiInterface::begin(Channel inChannel)
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


// -----------------------------------------------------------------------------
//                                 Output
// -----------------------------------------------------------------------------

#if MIDI_BUILD_OUTPUT

/*! \brief Generate and send a MIDI message from the values given.
 \param inType    The message type (see type defines for reference)
 \param inData1   The first data byte.
 \param inData2   The second data byte (if the message contains only 1 data byte,
 set this one to 0).
 \param inChannel The output channel on which the message will be sent 
 (values from 1 to 16). Note: you cannot send to OMNI.
 
 This is an internal method, use it only if you need to send raw data
 from your code, at your own risks.
 */
void MidiInterface::send(MidiType inType,
                         DataByte inData1,
                         DataByte inData2,
                         Channel inChannel)
{
    // Then test if channel is valid
    if (inChannel >= MIDI_CHANNEL_OFF  || 
        inChannel == MIDI_CHANNEL_OMNI || 
        inType < NoteOff)
    {
        
#if MIDI_USE_RUNNING_STATUS    
        mRunningStatus_TX = InvalidType;
#endif 
        
        return; // Don't send anything
    }
    
    if (inType <= PitchBend)  // Channel messages
    {
        // Protection: remove MSBs on data
        inData1 &= 0x7F;
        inData2 &= 0x7F;
        
        const StatusByte status = getStatus(inType, inChannel);
        
#if MIDI_USE_RUNNING_STATUS
        // Check Running Status
        if (mRunningStatus_TX != status)
        {
            // New message, memorise and send header
            mRunningStatus_TX = status;
            MIDI_SERIAL_PORT.write(mRunningStatus_TX);
        }
#else
        // Don't care about running status, send the status byte.
        MIDI_SERIAL_PORT.write(status);
#endif
        
        // Then send data
        MIDI_SERIAL_PORT.write(inData1);
        if (inType != ProgramChange && inType != AfterTouchChannel)
            MIDI_SERIAL_PORT.write(inData2);
        
        return;
    }
    else if (inType >= TuneRequest && inType <= SystemReset)
        sendRealTime(inType); // System Real-time and 1 byte.
}

#endif // MIDI_BUILD_OUTPUT


// -----------------------------------------------------------------------------
//                                  Input
// -----------------------------------------------------------------------------

#if MIDI_BUILD_INPUT

/*! \brief Read a MIDI message from the serial port
 using the main input channel (see setInputChannel() for reference).
 
 \return True if a valid message has been stored in the structure, false if not.
 A valid message is a message that matches the input channel. \n\n
 If the Thru is enabled and the messages matches the filter,
 it is sent back on the MIDI output.
 */
bool MidiInterface::read()
{
    return read(mInputChannel);
}

/*! \brief Reading/thru-ing method, the same as read()
 with a given input channel to read on. 
 */
bool MidiInterface::read(Channel inChannel)
{
    if (inChannel >= MIDI_CHANNEL_OFF)
        return false; // MIDI Input disabled.
    
    if (parse(inChannel))
    {
        if (inputFilter(inChannel))
        {
            
#if (MIDI_BUILD_OUTPUT && MIDI_BUILD_THRU)
            thruFilter(inChannel);
#endif
            
#if MIDI_USE_CALLBACKS
            launchCallback();
#endif
            return true;
        }
    }
    
    return false;
}

// -----------------------------------------------------------------------------

// Private method: MIDI parser
bool MidiInterface::parse(Channel inChannel)
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
                //resetInput(); 
                
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
                resetInput();
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
                        
                        resetInput();
                        return true;
                    }
                    else
                    {
                        // Well well well.. error.
                        resetInput();
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
                resetInput();
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
bool MidiInterface::inputFilter(Channel inChannel)
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
void MidiInterface::resetInput()
{
    mPendingMessageIndex = 0;
    mPendingMessageExpectedLenght = 0;
    mRunningStatus_RX = InvalidType;
}

// -----------------------------------------------------------------------------

#if MIDI_USE_CALLBACKS

// Private - launch callback function based on received type.
void MidiInterface::launchCallback()
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


// -----------------------------------------------------------------------------
//                                  Thru
// -----------------------------------------------------------------------------

#if MIDI_BUILD_THRU

// This method is called upon reception of a message 
// and takes care of Thru filtering and sending.

void MidiInterface::thruFilter(Channel inChannel)
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

#endif // MIDI_BUILD_THRU

END_MIDI_NAMESPACE
