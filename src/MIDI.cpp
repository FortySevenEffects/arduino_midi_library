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

#if MIDI_AUTO_INSTANCIATE
    #if MIDI_USE_SOFTWARE_SERIAL
        #include "../SoftwareSerial/SoftwareSerial.h"
        SoftwareSerial softSerialClass(MIDI_SOFTSERIAL_RX_PIN,
                                       MIDI_SOFTSERIAL_TX_PIN);
        #undef  MIDI_SERIAL_PORT
        #define MIDI_SERIAL_PORT softSerialClass
    #else
        #include "HardwareSerial.h"
    #endif // MIDI_USE_SOFTWARE_SERIAL

    MIDI_NAMESPACE::MidiInterface<MIDI_SERIAL_PORT> MIDI;

#endif // MIDI_AUTO_INSTANCIATE

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE
