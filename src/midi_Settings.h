/*!
 *  @file       midi_Settings.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Settings
 *  @version    3.5
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#pragma once

#include "midi_Namespace.h"

BEGIN_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

// Here are a few settings you can change to customize
// the library for your own project. You can for example
// choose to compile only parts of it so you gain flash
// space and optimise the speed of your sketch.

// -----------------------------------------------------------------------------

// Compilation flags. Set them to 1 to build the associated feature
// (MIDI in, out, thru), or to 0 to disable the feature and save space.
// Note that the Thru can only work if in and out are enabled.

#define MIDI_BUILD_INPUT                1
#define MIDI_BUILD_OUTPUT               1
#define MIDI_BUILD_THRU                 1

#define MIDI_USE_CALLBACKS              1

// Create a MIDI object automatically on the port defined with MIDI_SERIAL_PORT.
#define MIDI_AUTO_INSTANCIATE           1

// -----------------------------------------------------------------------------
// Serial port configuration

// Set the default port to use for MIDI.
#define MIDI_SERIAL_PORT                Serial

// Software serial options
#define MIDI_USE_SOFTWARE_SERIAL        0

#if MIDI_USE_SOFTWARE_SERIAL
    #define MIDI_SOFTSERIAL_RX_PIN      1   // Pin number to use for MIDI Input
    #define MIDI_SOFTSERIAL_TX_PIN      2   // Pin number to use for MIDI Output
#endif

// -----------------------------------------------------------------------------
// Misc. options

// Running status enables short messages when sending multiple values
// of the same type and channel.
// Set to 0 if you have troubles controlling your hardware.
#define MIDI_USE_RUNNING_STATUS         1
#define MIDI_USE_1BYTE_PARSING          1

#define MIDI_BAUDRATE                   31250
#define MIDI_SYSEX_ARRAY_SIZE           255     // Maximum size is 65535 bytes.

END_MIDI_NAMESPACE
