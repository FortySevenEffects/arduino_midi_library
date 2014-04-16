/*!
 *  @file       midi_Settings.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Settings
 *  @version    4.1
 *  @author     Francois Best
 *  @date       24/02/11
 *  @license    GPL v3.0 - Copyright Forty Seven Effects 2014
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
// Note that thru can work only if input and output are enabled.

#define MIDI_BUILD_INPUT                1
#define MIDI_BUILD_OUTPUT               1
#define MIDI_BUILD_THRU                 1

#define MIDI_USE_CALLBACKS              1

// -----------------------------------------------------------------------------

// Create a MIDI object automatically on the port defined with MIDI_SERIAL_PORT.
#ifndef MIDI_AUTO_INSTANCIATE
#   ifdef ARDUINO
#       define MIDI_AUTO_INSTANCIATE    1
#   else
#       define MIDI_AUTO_INSTANCIATE    0   ///< @see MIDI_CREATE_INSTANCE
#   endif
#endif

// -----------------------------------------------------------------------------
// Default serial port configuration (if MIDI_AUTO_INSTANCIATE is set)

// Set the default port to use for MIDI.
#if MIDI_AUTO_INSTANCIATE
#   ifdef ARDUINO
#       include "Arduino.h"
#       ifdef USBCON
#           define MIDI_DEFAULT_SERIAL_PORT     Serial1 // For Leonardo
#       else
#           define MIDI_DEFAULT_SERIAL_PORT     Serial  // For other Arduinos
#       endif
#       define MIDI_DEFAULT_SERIAL_CLASS        HardwareSerial
#       include "HardwareSerial.h"
#   else
#       error Auto-instanciation disabled. Use MIDI_CREATE_INSTANCE macro.
#   endif
#endif

// -----------------------------------------------------------------------------
// Misc. options

// Running status enables short messages when sending multiple values
// of the same type and channel.
// Set to 0 if you have troubles controlling your hardware.
#define MIDI_USE_RUNNING_STATUS         1

// NoteOn with 0 velocity should be handled as NoteOf.
// Set to 1 to get NoteOff events when receiving null-velocity NoteOn messages.
// Set to 0 to get NoteOn  events when receiving null-velocity NoteOn messages.
#define MIDI_HANDLE_NULL_VELOCITY_NOTE_ON_AS_NOTE_OFF 1

// Setting this to 1 will make MIDI.read parse only one byte of data for each
// call when data is available. This can speed up your application if receiving
// a lot of traffic, but might induce MIDI Thru and treatment latency.
#define MIDI_USE_1BYTE_PARSING          1

#define MIDI_BAUDRATE                   31250
#define MIDI_SYSEX_ARRAY_SIZE           255     // Maximum size is 65535 bytes.

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE
