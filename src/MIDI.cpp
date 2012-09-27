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
#       ifndef FSE_AVR
#           include "../SoftwareSerial/SoftwareSerial.h"
            SoftwareSerial softSerialClass(MIDI_SOFTSERIAL_RX_PIN,
                                           MIDI_SOFTSERIAL_TX_PIN);
#       else
#           error Todo: implement SoftwareSerial for avr core.
#       endif
#       undef  MIDI_SERIAL_PORT
#       define MIDI_SERIAL_PORT softSerialClass
        MIDI_NAMESPACE::MidiInterface<SoftwareSerial> MIDI(MIDI_SERIAL_PORT);

#   else
#       ifdef FSE_AVR
#           include <hardware_Serial.h>
#       else
#           include "HardwareSerial.h"
#       endif
        MIDI_NAMESPACE::MidiInterface<HardwareSerial> MIDI((HardwareSerial&)Serial);
#   endif // MIDI_USE_SOFTWARE_SERIAL
#endif // MIDI_AUTO_INSTANCIATE

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
byte encodeSysEx(const byte* inData, byte* outSysEx, byte inLength)
{
    byte retlen = 0;
    byte compteur;
    byte count7 = 0;
    
    outSysEx[0] = 0;
    for (compteur = 0; compteur < inLength; compteur++) {
        byte c = inData[compteur] & 0x7F;
        byte msb = inData[compteur] >> 7;
        outSysEx[0] |= msb << count7;
        outSysEx[1 + count7] = c;
        
        if (count7++ == 6) {
            outSysEx += 8;
            retlen += 8;
            outSysEx[0] = 0;
            count7 = 0;
        }
    }
    return retlen + count7 + ((count7 != 0)?1:0);
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
byte decodeSysEx(const byte* inSysEx, byte* outData, byte inLength)
{
    byte cnt;
    byte cnt2 = 0;
    byte bits = 0;
    
    for (cnt = 0; cnt < inLength; ++cnt) {
        
        if ((cnt % 8) == 0) {
            bits = inSysEx[cnt];
        } 
        else {
            outData[cnt2++] = inSysEx[cnt] | ((bits & 1) << 7);
            bits >>= 1;
        }
        
    }
    return cnt2;
}

END_MIDI_NAMESPACE
