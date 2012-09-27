/*!
 *  @file       midi_Settings.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Settings
 *  @version    4.0
 *  @author     Francois Best 
 *  @date       24/02/11
 *  license     GPL Forty Seven Effects - 2011
 */

#pragma once

#include "midi_Namespace.h"

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

// -----------------------------------------------------------------------------

// Create a MIDI object automatically on the port defined with MIDI_SERIAL_PORT.
// You can turn this off by adding #define MIDI_AUTO_INSTANCIATE 0 just before
// including <MIDI.h> in your sketch.
#ifndef MIDI_AUTO_INSTANCIATE
#   ifndef FSE_AVR
#       define MIDI_AUTO_INSTANCIATE    1
#   endif
#endif

// -----------------------------------------------------------------------------
// Default serial port configuration (if MIDI_AUTO_INSTANCIATE is set)

// Set the default port to use for MIDI.
#if MIDI_AUTO_INSTANCIATE
#   define MIDI_DEFAULT_SERIAL_PORT     Serial
#   define MIDI_SERIAL_CLASS            HardwareSerial
#   include "Arduino.h"
#   include "HardwareSerial.h"
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

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE
