/*!
 *  @file       midi_Settings.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Settings
 *  @version    4.2
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

#include "midi_Defs.h"

BEGIN_MIDI_NAMESPACE

/*! \brief Default Settings for the MIDI Library.

 To change the default settings, don't edit them there, create a subclass and
 override the values in that subclass, then use the MIDI_CREATE_CUSTOM_INSTANCE
 macro to create your instance. The settings you don't override will keep their
 default value. Eg:
 \code{.cpp}
 struct MySettings : public midi::DefaultSettings
 {
    static const bool UseRunningStatus = false; // Messes with my old equipment!
 };

 MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, midi, MySettings);
 \endcode
 */
struct DefaultSettings
{
    /*! Running status enables short messages when sending multiple values
     of the same type and channel.\n
     Set to 0 if you have troubles controlling your hardware.
     */
    static const bool UseRunningStatus = true;

    /* NoteOn with 0 velocity should be handled as NoteOf.\n
     Set to 1 to get NoteOff events when receiving null-velocity NoteOn messages.\n
     Set to 0 to get NoteOn  events when receiving null-velocity NoteOn messages.
     */
    static const bool HandleNullVelocityNoteOnAsNoteOff = true;

    // Setting this to 1 will make MIDI.read parse only one byte of data for each
    // call when data is available. This can speed up your application if receiving
    // a lot of traffic, but might induce MIDI Thru and treatment latency.
    static const bool Use1ByteParsing = true;

    /*! Override the default MIDI baudrate to transmit over USB serial, to
    a decoding program such as Hairless MIDI (set baudrate to 115200)\n
    http://projectgus.github.io/hairless-midiserial/
    */
    static const long BaudRate = 31250;

    /*! Maximum size of SysEx receivable. Decrease to save RAM if you don't expect
    to receive SysEx, or adjust accordingly.
    */
    static const unsigned SysExMaxSize = 128;
};

END_MIDI_NAMESPACE
