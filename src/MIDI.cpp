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


END_MIDI_NAMESPACE
