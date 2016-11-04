/*!
 *  @file       MIDI.hpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Inline implementations
 *  @author     Francois Best
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

BEGIN_MIDI_NAMESPACE

/// \brief Constructor for MidiInterface.
template<class SerialPort, class Settings>
inline MidiInterface<SerialPort, Settings>::MidiInterface(SerialPort& inSerial)
    : mSerial(inSerial)
    , mInputChannel(0)
    , mRunningStatus_RX(InvalidType)
    , mRunningStatus_TX(InvalidType)
    , mPendingMessageExpectedLenght(0)
    , mPendingMessageIndex(0)
    , mCurrentRpnNumber(0xffff)
    , mCurrentNrpnNumber(0xffff)
    , mThruActivated(true)
    , mThruFilterMode(Thru::Full)
{
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
}

/*! \brief Destructor for MidiInterface.

 This is not really useful for the Arduino, as it is never called...
 */
template<class SerialPort, class Settings>
inline MidiInterface<SerialPort, Settings>::~MidiInterface()
{
}

// -----------------------------------------------------------------------------

/*! \brief Call the begin method in the setup() function of the Arduino.

 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::begin(Channel inChannel)
{
    // Initialise the Serial port
#if defined(FSE_AVR)
    mSerial. template open<Settings::BaudRate>();
#else
    mSerial.begin(Settings::BaudRate);
#endif

    mInputChannel = inChannel;
    mRunningStatus_TX = InvalidType;
    mRunningStatus_RX = InvalidType;

    mPendingMessageIndex = 0;
    mPendingMessageExpectedLenght = 0;

    mCurrentRpnNumber  = 0xffff;
    mCurrentNrpnNumber = 0xffff;

    mMessage.valid   = false;
    mMessage.type    = InvalidType;
    mMessage.channel = 0;
    mMessage.data1   = 0;
    mMessage.data2   = 0;

    mThruFilterMode = Thru::Full;
    mThruActivated  = true;
}

// -----------------------------------------------------------------------------
//                                 Output
// -----------------------------------------------------------------------------

/*! \addtogroup output
 @{
 */

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
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::send(MidiType inType,
                                               DataByte inData1,
                                               DataByte inData2,
                                               Channel inChannel)
{
    // Then test if channel is valid
    if (inChannel >= MIDI_CHANNEL_OFF  ||
        inChannel == MIDI_CHANNEL_OMNI ||
        inType < 0x80)
    {
        return; // Don't send anything
    }

    if (inType <= PitchBend)  // Channel messages
    {
        // Protection: remove MSBs on data
        inData1 &= 0x7f;
        inData2 &= 0x7f;

        const StatusByte status = getStatus(inType, inChannel);

        if (Settings::UseRunningStatus)
        {
            if (mRunningStatus_TX != status)
            {
                // New message, memorise and send header
                mRunningStatus_TX = status;
                mSerial.write(mRunningStatus_TX);
            }
        }
        else
        {
            // Don't care about running status, send the status byte.
            mSerial.write(status);
        }

        // Then send data
        mSerial.write(inData1);
        if (inType != ProgramChange && inType != AfterTouchChannel)
        {
            mSerial.write(inData2);
        }
    }
    else if (inType >= Clock && inType <= SystemReset)
    {
        sendRealTime(inType); // System Real-time and 1 byte.
    }
}

// -----------------------------------------------------------------------------

/*! \brief Send a Note On message
 \param inNoteNumber  Pitch value in the MIDI format (0 to 127).
 \param inVelocity    Note attack velocity (0 to 127). A NoteOn with 0 velocity
 is considered as a NoteOff.
 \param inChannel     The channel on which the message will be sent (1 to 16).

 Take a look at the values, names and frequencies of notes here:
 http://www.phys.unsw.edu.au/jw/notes.html
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendNoteOn(DataByte inNoteNumber,
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
 sending only NoteOn data. sendNoteOff will always send a real NoteOff message.
 Take a look at the values, names and frequencies of notes here:
 http://www.phys.unsw.edu.au/jw/notes.html
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendNoteOff(DataByte inNoteNumber,
                                                      DataByte inVelocity,
                                                      Channel inChannel)
{
    send(NoteOff, inNoteNumber, inVelocity, inChannel);
}

/*! \brief Send a Program Change message
 \param inProgramNumber The Program to select (0 to 127).
 \param inChannel       The channel on which the message will be sent (1 to 16).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendProgramChange(DataByte inProgramNumber,
                                                            Channel inChannel)
{
    send(ProgramChange, inProgramNumber, 0, inChannel);
}

/*! \brief Send a Control Change message
 \param inControlNumber The controller number (0 to 127).
 \param inControlValue  The value for the specified controller (0 to 127).
 \param inChannel       The channel on which the message will be sent (1 to 16).
 @see MidiControlChangeNumber
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendControlChange(DataByte inControlNumber,
                                                            DataByte inControlValue,
                                                            Channel inChannel)
{
    send(ControlChange, inControlNumber, inControlValue, inChannel);
}

/*! \brief Send a Polyphonic AfterTouch message (applies to a specified note)
 \param inNoteNumber  The note to apply AfterTouch to (0 to 127).
 \param inPressure    The amount of AfterTouch to apply (0 to 127).
 \param inChannel     The channel on which the message will be sent (1 to 16).
 Note: this method is deprecated and will be removed in a future revision of the
 library, @see sendAfterTouch to send polyphonic and monophonic AfterTouch messages.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendPolyPressure(DataByte inNoteNumber,
                                                           DataByte inPressure,
                                                           Channel inChannel)
{
    send(AfterTouchPoly, inNoteNumber, inPressure, inChannel);
}

/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param inPressure    The amount of AfterTouch to apply to all notes.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendAfterTouch(DataByte inPressure,
                                                         Channel inChannel)
{
    send(AfterTouchChannel, inPressure, 0, inChannel);
}

/*! \brief Send a Polyphonic AfterTouch message (applies to a specified note)
 \param inNoteNumber  The note to apply AfterTouch to (0 to 127).
 \param inPressure    The amount of AfterTouch to apply (0 to 127).
 \param inChannel     The channel on which the message will be sent (1 to 16).
 @see Replaces sendPolyPressure (which is now deprecated).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendAfterTouch(DataByte inNoteNumber,
                                                         DataByte inPressure,
                                                         Channel inChannel)
{
    send(AfterTouchPoly, inNoteNumber, inPressure, inChannel);
}

/*! \brief Send a Pitch Bend message using a signed integer value.
 \param inPitchValue  The amount of bend to send (in a signed integer format),
 between MIDI_PITCHBEND_MIN and MIDI_PITCHBEND_MAX,
 center value is 0.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendPitchBend(int inPitchValue,
                                                        Channel inChannel)
{
    const unsigned bend = inPitchValue - MIDI_PITCHBEND_MIN;
    send(PitchBend, (bend & 0x7f), (bend >> 7) & 0x7f, inChannel);
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param inPitchValue  The amount of bend to send (in a floating point format),
 between -1.0f (maximum downwards bend)
 and +1.0f (max upwards bend), center value is 0.0f.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendPitchBend(double inPitchValue,
                                                        Channel inChannel)
{
    const int scale = inPitchValue > 0.0 ? MIDI_PITCHBEND_MAX : MIDI_PITCHBEND_MIN;
    const int value = int(inPitchValue * double(scale));
    sendPitchBend(value, inChannel);
}

/*! \brief Generate and send a System Exclusive frame.
 \param inLength  The size of the array to send
 \param inArray   The byte array containing the data to send
 \param inArrayContainsBoundaries When set to 'true', 0xf0 & 0xf7 bytes
 (start & stop SysEx) will NOT be sent
 (and therefore must be included in the array).
 default value for ArrayContainsBoundaries is set to 'false' for compatibility
 with previous versions of the library.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendSysEx(unsigned inLength,
                                                    const byte* inArray,
                                                    bool inArrayContainsBoundaries)
{
    const bool writeBeginEndBytes = !inArrayContainsBoundaries;

    if (writeBeginEndBytes)
    {
        mSerial.write(0xf0);
    }

    for (unsigned i = 0; i < inLength; ++i)
    {
        mSerial.write(inArray[i]);
    }

    if (writeBeginEndBytes)
    {
        mSerial.write(0xf7);
    }

    if (Settings::UseRunningStatus)
    {
        mRunningStatus_TX = InvalidType;
    }
}

/*! \brief Send a Tune Request message.

 When a MIDI unit receives this message,
 it should tune its oscillators (if equipped with any).
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendTuneRequest()
{
    mSerial.write(TuneRequest);

    if (Settings::UseRunningStatus)
    {
        mRunningStatus_TX = InvalidType;
    }
}

/*! \brief Send a MIDI Time Code Quarter Frame.

 \param inTypeNibble      MTC type
 \param inValuesNibble    MTC data
 See MIDI Specification for more information.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendTimeCodeQuarterFrame(DataByte inTypeNibble,
                                                                   DataByte inValuesNibble)
{
    const byte data = (((inTypeNibble & 0x07) << 4) | (inValuesNibble & 0x0f));
    sendTimeCodeQuarterFrame(data);
}

/*! \brief Send a MIDI Time Code Quarter Frame.

 See MIDI Specification for more information.
 \param inData  if you want to encode directly the nibbles in your program,
                you can send the byte here.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendTimeCodeQuarterFrame(DataByte inData)
{
    mSerial.write((byte)TimeCodeQuarterFrame);
    mSerial.write(inData);

    if (Settings::UseRunningStatus)
    {
        mRunningStatus_TX = InvalidType;
    }
}

/*! \brief Send a Song Position Pointer message.
 \param inBeats    The number of beats since the start of the song.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendSongPosition(unsigned inBeats)
{
    mSerial.write((byte)SongPosition);
    mSerial.write(inBeats & 0x7f);
    mSerial.write((inBeats >> 7) & 0x7f);

    if (Settings::UseRunningStatus)
    {
        mRunningStatus_TX = InvalidType;
    }
}

/*! \brief Send a Song Select message */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendSongSelect(DataByte inSongNumber)
{
    mSerial.write((byte)SongSelect);
    mSerial.write(inSongNumber & 0x7f);

    if (Settings::UseRunningStatus)
    {
        mRunningStatus_TX = InvalidType;
    }
}

/*! \brief Send a Real Time (one byte) message.

 \param inType    The available Real Time types are:
 Start, Stop, Continue, Clock, ActiveSensing and SystemReset.
 @see MidiType
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::sendRealTime(MidiType inType)
{
    // Do not invalidate Running Status for real-time messages
    // as they can be interleaved within any message.

    switch (inType)
    {
        case Clock:
        case Start:
        case Stop:
        case Continue:
        case ActiveSensing:
        case SystemReset:
            mSerial.write((byte)inType);
            break;
        default:
            // Invalid Real Time marker
            break;
    }
}

/*! \brief Start a Registered Parameter Number frame.
 \param inNumber The 14-bit number of the RPN you want to select.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::beginRpn(unsigned inNumber,
                                                          Channel inChannel)
{
    if (mCurrentRpnNumber != inNumber)
    {
        const byte numMsb = 0x7f & (inNumber >> 7);
        const byte numLsb = 0x7f & inNumber;
        sendControlChange(RPNLSB, numLsb, inChannel);
        sendControlChange(RPNMSB, numMsb, inChannel);
        mCurrentRpnNumber = inNumber;
    }
}

/*! \brief Send a 14-bit value for the currently selected RPN number.
 \param inValue  The 14-bit value of the selected RPN.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendRpnValue(unsigned inValue,
                                                              Channel inChannel)
{;
    const byte valMsb = 0x7f & (inValue >> 7);
    const byte valLsb = 0x7f & inValue;
    sendControlChange(DataEntryMSB, valMsb, inChannel);
    sendControlChange(DataEntryLSB, valLsb, inChannel);
}

/*! \brief Send separate MSB/LSB values for the currently selected RPN number.
 \param inMsb The MSB part of the value to send. Meaning depends on RPN number.
 \param inLsb The LSB part of the value to send. Meaning depends on RPN number.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendRpnValue(byte inMsb,
                                                              byte inLsb,
                                                              Channel inChannel)
{
    sendControlChange(DataEntryMSB, inMsb, inChannel);
    sendControlChange(DataEntryLSB, inLsb, inChannel);
}

/* \brief Increment the value of the currently selected RPN number by the specified amount.
 \param inAmount The amount to add to the currently selected RPN value.
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendRpnIncrement(byte inAmount,
                                                                  Channel inChannel)
{
    sendControlChange(DataIncrement, inAmount, inChannel);
}

/* \brief Decrement the value of the currently selected RPN number by the specified amount.
 \param inAmount The amount to subtract to the currently selected RPN value.
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendRpnDecrement(byte inAmount,
                                                                  Channel inChannel)
{
    sendControlChange(DataDecrement, inAmount, inChannel);
}

/*! \brief Terminate an RPN frame.
This will send a Null Function to deselect the currently selected RPN.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::endRpn(Channel inChannel)
{
    sendControlChange(RPNLSB, 0x7f, inChannel);
    sendControlChange(RPNMSB, 0x7f, inChannel);
    mCurrentRpnNumber = 0xffff;
}



/*! \brief Start a Non-Registered Parameter Number frame.
 \param inNumber The 14-bit number of the NRPN you want to select.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::beginNrpn(unsigned inNumber,
                                                           Channel inChannel)
{
    if (mCurrentNrpnNumber != inNumber)
    {
        const byte numMsb = 0x7f & (inNumber >> 7);
        const byte numLsb = 0x7f & inNumber;
        sendControlChange(NRPNLSB, numLsb, inChannel);
        sendControlChange(NRPNMSB, numMsb, inChannel);
        mCurrentNrpnNumber = inNumber;
    }
}

/*! \brief Send a 14-bit value for the currently selected NRPN number.
 \param inValue  The 14-bit value of the selected NRPN.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendNrpnValue(unsigned inValue,
                                                               Channel inChannel)
{;
    const byte valMsb = 0x7f & (inValue >> 7);
    const byte valLsb = 0x7f & inValue;
    sendControlChange(DataEntryMSB, valMsb, inChannel);
    sendControlChange(DataEntryLSB, valLsb, inChannel);
}

/*! \brief Send separate MSB/LSB values for the currently selected NRPN number.
 \param inMsb The MSB part of the value to send. Meaning depends on NRPN number.
 \param inLsb The LSB part of the value to send. Meaning depends on NRPN number.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendNrpnValue(byte inMsb,
                                                               byte inLsb,
                                                               Channel inChannel)
{
    sendControlChange(DataEntryMSB, inMsb, inChannel);
    sendControlChange(DataEntryLSB, inLsb, inChannel);
}

/* \brief Increment the value of the currently selected NRPN number by the specified amount.
 \param inAmount The amount to add to the currently selected NRPN value.
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendNrpnIncrement(byte inAmount,
                                                                   Channel inChannel)
{
    sendControlChange(DataIncrement, inAmount, inChannel);
}

/* \brief Decrement the value of the currently selected NRPN number by the specified amount.
 \param inAmount The amount to subtract to the currently selected NRPN value.
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::sendNrpnDecrement(byte inAmount,
                                                                   Channel inChannel)
{
    sendControlChange(DataDecrement, inAmount, inChannel);
}

/*! \brief Terminate an NRPN frame.
This will send a Null Function to deselect the currently selected NRPN.
 \param inChannel The channel on which the message will be sent (1 to 16).
*/
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::endNrpn(Channel inChannel)
{
    sendControlChange(NRPNLSB, 0x7f, inChannel);
    sendControlChange(NRPNMSB, 0x7f, inChannel);
    mCurrentNrpnNumber = 0xffff;
}

/*! @} */ // End of doc group MIDI Output

// -----------------------------------------------------------------------------

template<class SerialPort, class Settings>
StatusByte MidiInterface<SerialPort, Settings>::getStatus(MidiType inType,
                                                          Channel inChannel) const
{
    return ((byte)inType | ((inChannel - 1) & 0x0f));
}

// -----------------------------------------------------------------------------
//                                  Input
// -----------------------------------------------------------------------------

/*! \addtogroup input
 @{
*/

/*! \brief Read messages from the serial port using the main input channel.

 \return True if a valid message has been stored in the structure, false if not.
 A valid message is a message that matches the input channel. \n\n
 If the Thru is enabled and the message matches the filter,
 it is sent back on the MIDI output.
 @see see setInputChannel()
 */
template<class SerialPort, class Settings>
inline bool MidiInterface<SerialPort, Settings>::read()
{
    return read(mInputChannel);
}

/*! \brief Read messages on a specified channel.
 */
template<class SerialPort, class Settings>
inline bool MidiInterface<SerialPort, Settings>::read(Channel inChannel)
{
    if (inChannel >= MIDI_CHANNEL_OFF)
        return false; // MIDI Input disabled.

    if (!parse())
        return false;

    handleNullVelocityNoteOnAsNoteOff();
    const bool channelMatch = inputFilter(inChannel);

    if (channelMatch)
    {
        launchCallback();
    }

    thruFilter(inChannel);

    return channelMatch;
}

// -----------------------------------------------------------------------------

// Private method: MIDI parser
template<class SerialPort, class Settings>
bool MidiInterface<SerialPort, Settings>::parse()
{
    if (mSerial.available() == 0)
        // No data available.
        return false;

    // Parsing algorithm:
    // Get a byte from the serial buffer.
    // If there is no pending message to be recomposed, start a new one.
    //  - Find type and channel (if pertinent)
    //  - Look for other bytes in buffer, call parser recursively,
    //    until the message is assembled or the buffer is empty.
    // Else, add the extracted byte to the pending message, and check validity.
    // When the message is done, store it.

    const byte extracted = mSerial.read();

    // Ignore Undefined
    if (extracted == 0xf9 || extracted == 0xfd)
    {
        if (Settings::Use1ByteParsing)
        {
            return false;
        }
        else
        {
            return parse();
        }
    }

    if (mPendingMessageIndex == 0)
    {
        // Start a new pending message
        mPendingMessage[0] = extracted;

        // Check for running status first
        if (isChannelMessage(getTypeFromStatusByte(mRunningStatus_RX)))
        {
            // Only these types allow Running Status

            // If the status byte is not received, prepend it
            // to the pending message
            if (extracted < 0x80)
            {
                mPendingMessage[0]   = mRunningStatus_RX;
                mPendingMessage[1]   = extracted;
                mPendingMessageIndex = 1;
            }
            // Else: well, we received another status byte,
            // so the running status does not apply here.
            // It will be updated upon completion of this message.
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
                mMessage.type    = getTypeFromStatusByte(mPendingMessage[0]);
                mMessage.channel = 0;
                mMessage.data1   = 0;
                mMessage.data2   = 0;
                mMessage.valid   = true;

                // Do not reset all input attributes, Running Status must remain unchanged.
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
                // between 3 and MidiMessage::sSysExMaxSize bytes
                mPendingMessageExpectedLenght = MidiMessage::sSysExMaxSize;
                mRunningStatus_RX = InvalidType;
                mMessage.sysexArray[0] = SystemExclusive;
                break;

            case InvalidType:
            default:
                // This is obviously wrong. Let's get the hell out'a here.
                resetInput();
                return false;
                break;
        }

        if (mPendingMessageIndex >= (mPendingMessageExpectedLenght - 1))
        {
            // Reception complete
            mMessage.type    = getTypeFromStatusByte(mPendingMessage[0]);
            mMessage.channel = getChannelFromStatusByte(mPendingMessage[0]);
            mMessage.data1   = mPendingMessage[1];
            mMessage.data2   = 0; // Completed new message has 1 data byte

            mPendingMessageIndex = 0;
            mPendingMessageExpectedLenght = 0;
            mMessage.valid = true;
            return true;
        }
        else
        {
            // Waiting for more data
            mPendingMessageIndex++;
        }

        if (Settings::Use1ByteParsing)
        {
            // Message is not complete.
            return false;
        }
        else
        {
            // Call the parser recursively
            // to parse the rest of the message.
            return parse();
        }
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

                    mMessage.type    = (MidiType)extracted;
                    mMessage.data1   = 0;
                    mMessage.data2   = 0;
                    mMessage.channel = 0;
                    mMessage.valid   = true;
                    return true;

                    // End of Exclusive
                case 0xf7:
                    if (mMessage.sysexArray[0] == SystemExclusive)
                    {
                        // Store the last byte (EOX)
                        mMessage.sysexArray[mPendingMessageIndex++] = 0xf7;
                        mMessage.type = SystemExclusive;

                        // Get length
                        mMessage.data1   = mPendingMessageIndex & 0xff; // LSB
                        mMessage.data2   = mPendingMessageIndex >> 8;   // MSB
                        mMessage.channel = 0;
                        mMessage.valid   = true;

                        resetInput();
                        return true;
                    }
                    else
                    {
                        // Well well well.. error.
                        resetInput();
                        return false;
                    }

                default:
                    break; // LCOV_EXCL_LINE - Coverage blind spot
            }
        }

        // Add extracted data byte to pending message
        if (mPendingMessage[0] == SystemExclusive)
            mMessage.sysexArray[mPendingMessageIndex] = extracted;
        else
            mPendingMessage[mPendingMessageIndex] = extracted;

        // Now we are going to check if we have reached the end of the message
        if (mPendingMessageIndex >= (mPendingMessageExpectedLenght - 1))
        {
            // "FML" case: fall down here with an overflown SysEx..
            // This means we received the last possible data byte that can fit
            // the buffer. If this happens, try increasing MidiMessage::sSysExMaxSize.
            if (mPendingMessage[0] == SystemExclusive)
            {
                resetInput();
                return false;
            }

            mMessage.type = getTypeFromStatusByte(mPendingMessage[0]);

            if (isChannelMessage(mMessage.type))
                mMessage.channel = getChannelFromStatusByte(mPendingMessage[0]);
            else
                mMessage.channel = 0;

            mMessage.data1 = mPendingMessage[1];

            // Save data2 only if applicable
            mMessage.data2 = mPendingMessageExpectedLenght == 3 ? mPendingMessage[2] : 0;

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

            if (Settings::Use1ByteParsing)
            {
                // Message is not complete.
                return false;
            }
            else
            {
                // Call the parser recursively to parse the rest of the message.
                return parse();
            }
        }
    }
}

// Private method, see midi_Settings.h for documentation
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::handleNullVelocityNoteOnAsNoteOff()
{
    if (Settings::HandleNullVelocityNoteOnAsNoteOff &&
        getType() == NoteOn && getData2() == 0)
    {
        mMessage.type = NoteOff;
    }
}

// Private method: check if the received message is on the listened channel
template<class SerialPort, class Settings>
inline bool MidiInterface<SerialPort, Settings>::inputFilter(Channel inChannel)
{
    // This method handles recognition of channel
    // (to know if the message is destinated to the Arduino)

    // First, check if the received message is Channel
    if (mMessage.type >= NoteOff && mMessage.type <= PitchBend)
    {
        // Then we need to know if we listen to it
        if ((mMessage.channel == inChannel) ||
            (inChannel == MIDI_CHANNEL_OMNI))
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
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::resetInput()
{
    mPendingMessageIndex = 0;
    mPendingMessageExpectedLenght = 0;
    mRunningStatus_RX = InvalidType;
}

// -----------------------------------------------------------------------------

/*! \brief Get the last received message's type

 Returns an enumerated type. @see MidiType
 */
template<class SerialPort, class Settings>
inline MidiType MidiInterface<SerialPort, Settings>::getType() const
{
    return mMessage.type;
}

/*! \brief Get the channel of the message stored in the structure.

 \return Channel range is 1 to 16.
 For non-channel messages, this will return 0.
 */
template<class SerialPort, class Settings>
inline Channel MidiInterface<SerialPort, Settings>::getChannel() const
{
    return mMessage.channel;
}

/*! \brief Get the first data byte of the last received message. */
template<class SerialPort, class Settings>
inline DataByte MidiInterface<SerialPort, Settings>::getData1() const
{
    return mMessage.data1;
}

/*! \brief Get the second data byte of the last received message. */
template<class SerialPort, class Settings>
inline DataByte MidiInterface<SerialPort, Settings>::getData2() const
{
    return mMessage.data2;
}

/*! \brief Get the System Exclusive byte array.

 @see getSysExArrayLength to get the array's length in bytes.
 */
template<class SerialPort, class Settings>
inline const byte* MidiInterface<SerialPort, Settings>::getSysExArray() const
{
    return mMessage.sysexArray;
}

/*! \brief Get the lenght of the System Exclusive array.

 It is coded using data1 as LSB and data2 as MSB.
 \return The array's length, in bytes.
 */
template<class SerialPort, class Settings>
inline unsigned MidiInterface<SerialPort, Settings>::getSysExArrayLength() const
{
    return mMessage.getSysExSize();
}

/*! \brief Check if a valid message is stored in the structure. */
template<class SerialPort, class Settings>
inline bool MidiInterface<SerialPort, Settings>::check() const
{
    return mMessage.valid;
}

// -----------------------------------------------------------------------------

template<class SerialPort, class Settings>
inline Channel MidiInterface<SerialPort, Settings>::getInputChannel() const
{
    return mInputChannel;
}

/*! \brief Set the value for the input MIDI channel
 \param inChannel the channel value. Valid values are 1 to 16, MIDI_CHANNEL_OMNI
 if you want to listen to all channels, and MIDI_CHANNEL_OFF to disable input.
 */
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::setInputChannel(Channel inChannel)
{
    mInputChannel = inChannel;
}

// -----------------------------------------------------------------------------

/*! \brief Extract an enumerated MIDI type from a status byte.

 This is a utility static method, used internally,
 made public so you can handle MidiTypes more easily.
 */
template<class SerialPort, class Settings>
MidiType MidiInterface<SerialPort, Settings>::getTypeFromStatusByte(byte inStatus)
{
    if ((inStatus  < 0x80) ||
        (inStatus == 0xf4) ||
        (inStatus == 0xf5) ||
        (inStatus == 0xf9) ||
        (inStatus == 0xfD))
    {
        // Data bytes and undefined.
        return InvalidType;
    }
    if (inStatus < 0xf0)
    {
        // Channel message, remove channel nibble.
        return MidiType(inStatus & 0xf0);
    }

    return MidiType(inStatus);
}

/*! \brief Returns channel in the range 1-16
 */
template<class SerialPort, class Settings>
inline Channel MidiInterface<SerialPort, Settings>::getChannelFromStatusByte(byte inStatus)
{
    return (inStatus & 0x0f) + 1;
}

template<class SerialPort, class Settings>
bool MidiInterface<SerialPort, Settings>::isChannelMessage(MidiType inType)
{
    return (inType == NoteOff           ||
            inType == NoteOn            ||
            inType == ControlChange     ||
            inType == AfterTouchPoly    ||
            inType == AfterTouchChannel ||
            inType == PitchBend         ||
            inType == ProgramChange);
}

// -----------------------------------------------------------------------------

/*! \addtogroup callbacks
 @{
 */

template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleNoteOff(void (*fptr)(byte channel, byte note, byte velocity))          { mNoteOffCallback              = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleNoteOn(void (*fptr)(byte channel, byte note, byte velocity))           { mNoteOnCallback               = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure))   { mAfterTouchPolyCallback       = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleControlChange(void (*fptr)(byte channel, byte number, byte value))     { mControlChangeCallback        = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleProgramChange(void (*fptr)(byte channel, byte number))                 { mProgramChangeCallback        = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleAfterTouchChannel(void (*fptr)(byte channel, byte pressure))           { mAfterTouchChannelCallback    = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandlePitchBend(void (*fptr)(byte channel, int bend))                        { mPitchBendCallback            = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleSystemExclusive(void (*fptr)(byte* array, unsigned size))              { mSystemExclusiveCallback      = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleTimeCodeQuarterFrame(void (*fptr)(byte data))                          { mTimeCodeQuarterFrameCallback = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleSongPosition(void (*fptr)(unsigned beats))                             { mSongPositionCallback         = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleSongSelect(void (*fptr)(byte songnumber))                              { mSongSelectCallback           = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleTuneRequest(void (*fptr)(void))                                        { mTuneRequestCallback          = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleClock(void (*fptr)(void))                                              { mClockCallback                = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleStart(void (*fptr)(void))                                              { mStartCallback                = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleContinue(void (*fptr)(void))                                           { mContinueCallback             = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleStop(void (*fptr)(void))                                               { mStopCallback                 = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleActiveSensing(void (*fptr)(void))                                      { mActiveSensingCallback        = fptr; }
template<class SerialPort, class Settings> void MidiInterface<SerialPort, Settings>::setHandleSystemReset(void (*fptr)(void))                                        { mSystemResetCallback          = fptr; }

/*! \brief Detach an external function from the given type.

 Use this method to cancel the effects of setHandle********.
 \param inType        The type of message to unbind.
 When a message of this type is received, no function will be called.
 */
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::disconnectCallbackFromType(MidiType inType)
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

/*! @} */ // End of doc group MIDI Callbacks

// Private - launch callback function based on received type.
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::launchCallback()
{
    // The order is mixed to allow frequent messages to trigger their callback faster.
    switch (mMessage.type)
    {
            // Notes
        case NoteOff:               if (mNoteOffCallback != 0)               mNoteOffCallback(mMessage.channel, mMessage.data1, mMessage.data2);   break;
        case NoteOn:                if (mNoteOnCallback != 0)                mNoteOnCallback(mMessage.channel, mMessage.data1, mMessage.data2);    break;

            // Real-time messages
        case Clock:                 if (mClockCallback != 0)                 mClockCallback();           break;
        case Start:                 if (mStartCallback != 0)                 mStartCallback();           break;
        case Continue:              if (mContinueCallback != 0)              mContinueCallback();        break;
        case Stop:                  if (mStopCallback != 0)                  mStopCallback();            break;
        case ActiveSensing:         if (mActiveSensingCallback != 0)         mActiveSensingCallback();   break;

            // Continuous controllers
        case ControlChange:         if (mControlChangeCallback != 0)         mControlChangeCallback(mMessage.channel, mMessage.data1, mMessage.data2);    break;
        case PitchBend:             if (mPitchBendCallback != 0)             mPitchBendCallback(mMessage.channel, (int)((mMessage.data1 & 0x7f) | ((mMessage.data2 & 0x7f) << 7)) + MIDI_PITCHBEND_MIN); break; // TODO: check this
        case AfterTouchPoly:        if (mAfterTouchPolyCallback != 0)        mAfterTouchPolyCallback(mMessage.channel, mMessage.data1, mMessage.data2);    break;
        case AfterTouchChannel:     if (mAfterTouchChannelCallback != 0)     mAfterTouchChannelCallback(mMessage.channel, mMessage.data1);    break;

        case ProgramChange:         if (mProgramChangeCallback != 0)         mProgramChangeCallback(mMessage.channel, mMessage.data1);    break;
        case SystemExclusive:       if (mSystemExclusiveCallback != 0)       mSystemExclusiveCallback(mMessage.sysexArray, mMessage.getSysExSize());    break;

            // Occasional messages
        case TimeCodeQuarterFrame:  if (mTimeCodeQuarterFrameCallback != 0)  mTimeCodeQuarterFrameCallback(mMessage.data1);    break;
        case SongPosition:          if (mSongPositionCallback != 0)          mSongPositionCallback((mMessage.data1 & 0x7f) | ((mMessage.data2 & 0x7f) << 7));    break;
        case SongSelect:            if (mSongSelectCallback != 0)            mSongSelectCallback(mMessage.data1);    break;
        case TuneRequest:           if (mTuneRequestCallback != 0)           mTuneRequestCallback();    break;

        case SystemReset:           if (mSystemResetCallback != 0)           mSystemResetCallback();    break;

        case InvalidType:
        default:
            break; // LCOV_EXCL_LINE - Unreacheable code, but prevents unhandled case warning.
    }
}

/*! @} */ // End of doc group MIDI Input

// -----------------------------------------------------------------------------
//                                  Thru
// -----------------------------------------------------------------------------

/*! \addtogroup thru
 @{
 */

/*! \brief Set the filter for thru mirroring
 \param inThruFilterMode a filter mode

 @see Thru::Mode
 */
template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::setThruFilterMode(Thru::Mode inThruFilterMode)
{
    mThruFilterMode = inThruFilterMode;
    mThruActivated  = mThruFilterMode != Thru::Off;
}

template<class SerialPort, class Settings>
inline Thru::Mode MidiInterface<SerialPort, Settings>::getFilterMode() const
{
    return mThruFilterMode;
}

template<class SerialPort, class Settings>
inline bool MidiInterface<SerialPort, Settings>::getThruState() const
{
    return mThruActivated;
}

template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::turnThruOn(Thru::Mode inThruFilterMode)
{
    mThruActivated = true;
    mThruFilterMode = inThruFilterMode;
}

template<class SerialPort, class Settings>
inline void MidiInterface<SerialPort, Settings>::turnThruOff()
{
    mThruActivated = false;
    mThruFilterMode = Thru::Off;
}

/*! @} */ // End of doc group MIDI Thru

// This method is called upon reception of a message
// and takes care of Thru filtering and sending.
// - All system messages (System Exclusive, Common and Real Time) are passed
//   to output unless filter is set to Off.
// - Channel messages are passed to the output whether their channel
//   is matching the input channel and the filter setting
template<class SerialPort, class Settings>
void MidiInterface<SerialPort, Settings>::thruFilter(Channel inChannel)
{
    // If the feature is disabled, don't do anything.
    if (!mThruActivated || (mThruFilterMode == Thru::Off))
        return;

    // First, check if the received message is Channel
    if (mMessage.type >= NoteOff && mMessage.type <= PitchBend)
    {
        const bool filter_condition = ((mMessage.channel == inChannel) ||
                                       (inChannel == MIDI_CHANNEL_OMNI));

        // Now let's pass it to the output
        switch (mThruFilterMode)
        {
            case Thru::Full:
                send(mMessage.type,
                     mMessage.data1,
                     mMessage.data2,
                     mMessage.channel);
                break;

            case Thru::SameChannel:
                if (filter_condition)
                {
                    send(mMessage.type,
                         mMessage.data1,
                         mMessage.data2,
                         mMessage.channel);
                }
                break;

            case Thru::DifferentChannel:
                if (!filter_condition)
                {
                    send(mMessage.type,
                         mMessage.data1,
                         mMessage.data2,
                         mMessage.channel);
                }
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
                break;

            case SystemExclusive:
                // Send SysEx (0xf0 and 0xf7 are included in the buffer)
                sendSysEx(getSysExArrayLength(), getSysExArray(), true);
                break;

            case SongSelect:
                sendSongSelect(mMessage.data1);
                break;

            case SongPosition:
                sendSongPosition(mMessage.data1 | ((unsigned)mMessage.data2 << 7));
                break;

            case TimeCodeQuarterFrame:
                sendTimeCodeQuarterFrame(mMessage.data1,mMessage.data2);
                break;

            default:
                break; // LCOV_EXCL_LINE - Unreacheable code, but prevents unhandled case warning.
        }
    }
}

END_MIDI_NAMESPACE
