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

#if MIDI_AUTO_INSTANCIATE && defined(ARDUINO)
    MIDI_CREATE_INSTANCE(MIDI_DEFAULT_SERIAL_CLASS,
                         MIDI_DEFAULT_SERIAL_PORT,
                         MIDI);
#endif

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

/*! \brief Encode System Exclusive messages.
 SysEx messages are encoded to guarantee transmission of data bytes higher than
 127 without breaking the MIDI protocol. Use this static method to convert the
 data you want to send.
 \param inData The data to encode.
 \param outSysEx The output buffer where to store the encoded message.
 \param inLength The lenght of the input buffer.
 \return The lenght of the encoded output buffer.
 @see decodeSysEx
 */
unsigned encodeSysEx(const byte* inData, byte* outSysEx, unsigned inLength)
{
    unsigned outLength  = 0;     // Num bytes in output array.
    byte count          = 0;     // Num 7bytes in a block.
    outSysEx[0]         = 0;

    for (unsigned i = 0; i < inLength; ++i)
    {
        const byte data = inData[i];
        const byte msb  = data >> 7;
        const byte body = data & 0x7f;

        outSysEx[0] |= (msb << count);
        outSysEx[1 + count] = body;

        if (count++ == 6)
        {
            outSysEx   += 8;
            outLength  += 8;
            outSysEx[0] = 0;
            count       = 0;
        }
    }
    return outLength + count + (count != 0 ? 1 : 0);
}

/*! \brief Decode System Exclusive messages.
 SysEx messages are encoded to guarantee transmission of data bytes higher than
 127 without breaking the MIDI protocol. Use this static method to reassemble
 your received message.
 \param inSysEx The SysEx data received from MIDI in.
 \param outData    The output buffer where to store the decrypted message.
 \param inLength The lenght of the input buffer.
 \return The lenght of the output buffer.
 @see encodeSysEx @see getSysExArrayLength
 */
unsigned decodeSysEx(const byte* inSysEx, byte* outData, unsigned inLength)
{
    unsigned count  = 0;
    byte msbStorage = 0;

    for (unsigned i = 0; i < inLength; ++i)
    {
        if ((i % 8) == 0)
        {
            msbStorage = inSysEx[i];
        }
        else
        {
            outData[count++] = inSysEx[i] | ((msbStorage & 1) << 7);
            msbStorage >>= 1;
        }
    }
    return count;
}

END_MIDI_NAMESPACE
