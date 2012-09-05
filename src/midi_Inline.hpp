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

#if MIDI_BUILD_OUTPUT

StatusByte MidiInterface::getStatus(kMIDIType inType,
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

#endif // MIDI_BUILD_INPUT

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

END_MIDI_NAMESPACE
