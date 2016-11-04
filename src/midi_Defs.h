/*!
 *  @file       midi_Defs.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Definitions
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

#include "midi_Namespace.h"

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

BEGIN_MIDI_NAMESPACE

#define MIDI_LIBRARY_VERSION        0x040300
#define MIDI_LIBRARY_VERSION_MAJOR  4
#define MIDI_LIBRARY_VERSION_MINOR  3
#define MIDI_LIBRARY_VERSION_PATCH  0

// -----------------------------------------------------------------------------

#define MIDI_CHANNEL_OMNI       0
#define MIDI_CHANNEL_OFF        17 // and over

#define MIDI_PITCHBEND_MIN      -8192
#define MIDI_PITCHBEND_MAX      8191

// -----------------------------------------------------------------------------
// Type definitions

typedef byte StatusByte;
typedef byte DataByte;
typedef byte Channel;
typedef byte FilterMode;

// -----------------------------------------------------------------------------

/*! Enumeration of MIDI types */
enum MidiType
{
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
};

// -----------------------------------------------------------------------------

/*! Enumeration of Thru filter modes */
struct Thru
{
    enum Mode
    {
        Off                   = 0,  ///< Thru disabled (nothing passes through).
        Full                  = 1,  ///< Fully enabled Thru (every incoming message is sent back).
        SameChannel           = 2,  ///< Only the messages on the Input Channel will be sent back.
        DifferentChannel      = 3,  ///< All the messages but the ones on the Input Channel will be sent back.
    };
};

/*! Deprecated: use Thru::Mode instead.
 Will be removed in v5.0.
*/
enum __attribute__ ((deprecated)) MidiFilterMode
{
    Off                 = Thru::Off,
    Full                = Thru::Full,
    SameChannel         = Thru::SameChannel,
    DifferentChannel    = Thru::DifferentChannel,
};

// -----------------------------------------------------------------------------

/*! \brief Enumeration of Control Change command numbers.
 See the detailed controllers numbers & description here:
 http://www.somascape.org/midi/tech/spec.html#ctrlnums
 */
enum MidiControlChangeNumber
{
    // High resolution Continuous Controllers MSB (+32 for LSB) ----------------
    BankSelect                  = 0,
    ModulationWheel             = 1,
    BreathController            = 2,
    // CC3 undefined
    FootController              = 4,
    PortamentoTime              = 5,
    DataEntryMSB                = 6,
    ChannelVolume               = 7,
    Balance                     = 8,
    // CC9 undefined
    Pan                         = 10,
    ExpressionController        = 11,
    EffectControl1              = 12,
    EffectControl2              = 13,
    // CC14 undefined
    // CC15 undefined
    GeneralPurposeController1   = 16,
    GeneralPurposeController2   = 17,
    GeneralPurposeController3   = 18,
    GeneralPurposeController4   = 19,

    DataEntryLSB                = 38,

    // Switches ----------------------------------------------------------------
    Sustain                     = 64,
    Portamento                  = 65,
    Sostenuto                   = 66,
    SoftPedal                   = 67,
    Legato                      = 68,
    Hold                        = 69,

    // Low resolution continuous controllers -----------------------------------
    SoundController1            = 70,   ///< Synth: Sound Variation   FX: Exciter On/Off
    SoundController2            = 71,   ///< Synth: Harmonic Content  FX: Compressor On/Off
    SoundController3            = 72,   ///< Synth: Release Time      FX: Distortion On/Off
    SoundController4            = 73,   ///< Synth: Attack Time       FX: EQ On/Off
    SoundController5            = 74,   ///< Synth: Brightness        FX: Expander On/Off
    SoundController6            = 75,   ///< Synth: Decay Time        FX: Reverb On/Off
    SoundController7            = 76,   ///< Synth: Vibrato Rate      FX: Delay On/Off
    SoundController8            = 77,   ///< Synth: Vibrato Depth     FX: Pitch Transpose On/Off
    SoundController9            = 78,   ///< Synth: Vibrato Delay     FX: Flange/Chorus On/Off
    SoundController10           = 79,   ///< Synth: Undefined         FX: Special Effects On/Off
    GeneralPurposeController5   = 80,
    GeneralPurposeController6   = 81,
    GeneralPurposeController7   = 82,
    GeneralPurposeController8   = 83,
    PortamentoControl           = 84,
    // CC85 to CC90 undefined
    Effects1                    = 91,   ///< Reverb send level
    Effects2                    = 92,   ///< Tremolo depth
    Effects3                    = 93,   ///< Chorus send level
    Effects4                    = 94,   ///< Celeste depth
    Effects5                    = 95,   ///< Phaser depth
    DataIncrement               = 96,
    DataDecrement               = 97,
    NRPNLSB                     = 98,   ///< Non-Registered Parameter Number (LSB)
    NRPNMSB                     = 99,   ///< Non-Registered Parameter Number (MSB)
    RPNLSB                      = 100,  ///< Registered Parameter Number (LSB)
    RPNMSB                      = 101,  ///< Registered Parameter Number (MSB)

    // Channel Mode messages ---------------------------------------------------
    AllSoundOff                 = 120,
    ResetAllControllers         = 121,
    LocalControl                = 122,
    AllNotesOff                 = 123,
    OmniModeOff                 = 124,
    OmniModeOn                  = 125,
    MonoModeOn                  = 126,
    PolyModeOn                  = 127
};

struct RPN
{
    enum RegisteredParameterNumbers
    {
        PitchBendSensitivity    = 0x0000,
        ChannelFineTuning       = 0x0001,
        ChannelCoarseTuning     = 0x0002,
        SelectTuningProgram     = 0x0003,
        SelectTuningBank        = 0x0004,
        ModulationDepthRange    = 0x0005,
        NullFunction            = (0x7f << 7) + 0x7f,
    };
};

// -----------------------------------------------------------------------------

/*! \brief Create an instance of the library attached to a serial port.
 You can use HardwareSerial or SoftwareSerial for the serial port.
 Example: MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);
 Then call midi2.begin(), midi2.read() etc..
 */
#define MIDI_CREATE_INSTANCE(Type, SerialPort, Name)                            \
    midi::MidiInterface<Type> Name((Type&)SerialPort);

#if defined(ARDUINO_SAM_DUE) || defined(USBCON)
    // Leonardo, Due and other USB boards use Serial1 by default.
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
    /*! \brief Create an instance of the library with default name, serial port
    and settings, for compatibility with sketches written with pre-v4.2 MIDI Lib,
    or if you don't bother using custom names, serial port or settings.
    */
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial,  MIDI);
#endif

/*! \brief Create an instance of the library attached to a serial port with
 custom settings.
 @see DefaultSettings
 @see MIDI_CREATE_INSTANCE
 */
#define MIDI_CREATE_CUSTOM_INSTANCE(Type, SerialPort, Name, Settings)           \
    midi::MidiInterface<Type, Settings> Name((Type&)SerialPort);

END_MIDI_NAMESPACE
