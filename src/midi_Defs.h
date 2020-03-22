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

#define MIDI_LIBRARY_VERSION        0x050000
#define MIDI_LIBRARY_VERSION_MAJOR  5
#define MIDI_LIBRARY_VERSION_MINOR  0
#define MIDI_LIBRARY_VERSION_PATCH  0

// -----------------------------------------------------------------------------

#define MIDI_CHANNEL_OMNI       0
#define MIDI_CHANNEL_OFF        17 // and over

#define MIDI_PITCHBEND_MIN      -8192
#define MIDI_PITCHBEND_MAX      8191

/*! Receiving Active Sensing 
*/
static const uint16_t ActiveSensingTimeout = 300;

// -----------------------------------------------------------------------------
// Type definitions

typedef byte StatusByte;
typedef byte DataByte;
typedef byte Channel;
typedef byte FilterMode;

// -----------------------------------------------------------------------------
// Errors
static const uint8_t ErrorParse = 0;
static const uint8_t ErrorActiveSensingTimeout = 1;

// -----------------------------------------------------------------------------
// Aliasing

using ErrorCallback                = void (*)(int8_t);
using NoteOffCallback              = void (*)(Channel channel, byte note, byte velocity);
using NoteOnCallback               = void (*)(Channel channel, byte note, byte velocity);
using AfterTouchPolyCallback       = void (*)(Channel channel, byte note, byte velocity);
using ControlChangeCallback        = void (*)(Channel channel, byte, byte);
using ProgramChangeCallback        = void (*)(Channel channel, byte);
using AfterTouchChannelCallback    = void (*)(Channel channel, byte);
using PitchBendCallback            = void (*)(Channel channel, int);
using SystemExclusiveCallback      = void (*)(byte * array, unsigned size);
using TimeCodeQuarterFrameCallback = void (*)(byte data);
using SongPositionCallback         = void (*)(unsigned beats);
using SongSelectCallback           = void (*)(byte songnumber);
using TuneRequestCallback          = void (*)(void);
using ClockCallback                = void (*)(void);
using StartCallback                = void (*)(void);
using ContinueCallback             = void (*)(void);
using StopCallback                 = void (*)(void);
using ActiveSensingCallback        = void (*)(void);
using SystemResetCallback          = void (*)(void);

// -----------------------------------------------------------------------------

/*! Enumeration of MIDI types */
enum MidiType: uint8_t
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
	SystemExclusiveStart  = SystemExclusive,   ///< System Exclusive Start
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    Undefined_F4          = 0xF4,
    Undefined_F5          = 0xF5,
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
	SystemExclusiveEnd    = 0xF7,    ///< System Exclusive End
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Undefined_F9          = 0xF9,
    Tick                  = Undefined_F9,
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    Undefined_FD          = 0xFD,
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

// -----------------------------------------------------------------------------

/*! \brief Enumeration of Control Change command numbers.
 See the detailed controllers numbers & description here:
 http://www.somascape.org/midi/tech/spec.html#ctrlnums
 */
enum MidiControlChangeNumber: uint8_t
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
    enum RegisteredParameterNumbers: uint16_t
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

/*! Manufacturer System Exclusive ID 
The MIDI Manufacturers Association (MMA) issues 
Manufacturer System Exclusive IDs to MIDI 
hardware and software producers world-wide (except Japan).
These MIDI ID numbers allow developers to uniquely 
identify their products and provide a means for 
communicating product-specific information via MIDI 
protocol (using SysEx messages). The MIDI 1.0 Detailed 
Specification describes the format of SysEx messages, 
as well as all other MIDI messages that a developer 
may need to know to maintain industry compatibility.

https://www.midi.org/request-sysex-id
*/
enum ShortManufacturerId : uint8_t
{
    SequentialCircuits  = 0x01,
    BigBriarIDP         = 0x02,
    OctavePlateau       = 0x03,
    MoogMusic           = 0x04,
    // ...
};

enum LongManufacturerId : uint16_t
{
    /* North American Manufacturers */
    TimeWarnerInteractive   = 0x01,
    /* European manufacturers */
    Dream                   = 0x2000,
    /* Japanese Manufacturers */
    CrimsonTechnologyInc    = 0x4000,
    // ...
};

template<typename T, typename S>
struct valueString {
    T  value;
    S strptr;
};

static const valueString<uint8_t, const char*> ManuShortValues[] = {
	{ SequentialCircuits,       "Sequential Circuits" },
	{ BigBriarIDP,              "Big Briar / IDP" },
	{ OctavePlateau,            "Voyetra / Octave-Plateau" },
	{ MoogMusic,                "Moog" },
};

static const valueString<uint16_t, const char*> ManuLongValues[] = {
	{ TimeWarnerInteractive,    "Time Warner Interactive" },
	{ Dream,                    "Dream" },
	{ CrimsonTechnologyInc,     "Crimson Technology Inc" },
};

static const valueString<uint8_t, const char*> NoteValues[] = {
	{ 0,								"C-1" },
	{ 1,								"C#-1" },
	{ 2,								"D-1" },
	{ 3,								"D#-1" },
	{ 4,								"E-1" },
	{ 5,								"F-1" },
	{ 6,								"F#-1" },
	{ 7,								"G-1" },
	{ 8,								"G#-1" },
	{ 9,								"A-1" },
	{ 10,								"A#-1" },
	{ 11,								"B-1" },
	{ 12,								"C0" },
	{ 13,								"C#0" },
	{ 14,								"D0" },
	{ 15,								"D#0" },
	{ 16,								"E0" },
	{ 17,								"F0" },
	{ 18,								"F#0" },
	{ 19,								"G0" },
	{ 20,								"G#0" },
	{ 21,								"A0" },
	{ 22,								"A#0" },
	{ 23,								"B0" },
	{ 24,								"C1" },
	{ 25,								"C#1" },
	{ 26,								"D1" },
	{ 27,								"D#1" },
	{ 28,								"E1" },
	{ 29,								"F1" },
	{ 30,								"F#1" },
	{ 31,								"G1" },
	{ 32,								"G#1" },
	{ 33,								"A1" },
	{ 34,								"A#1" },
	{ 35,								"B1" },
	{ 36,								"C2" },
	{ 37,								"C#2" },
	{ 38,								"D2" },
	{ 39,								"D#2" },
	{ 40,								"E2" },
	{ 41,								"F2" },
	{ 42,								"F#2" },
	{ 43,								"G2" },
	{ 44,								"G#2" },
	{ 45,								"A2" },
	{ 46,								"A#2" },
	{ 47,								"B2" },
	{ 48,								"C3" },
	{ 49,								"C#3" },
	{ 50,								"D3" },
	{ 51,								"D#3" },
	{ 52,								"E3" },
	{ 53,								"F3" },
	{ 54,								"F#3" },
	{ 55,								"G3" },
	{ 56,								"G#3" },
	{ 57,								"A3" },
	{ 58,								"A#3" },
	{ 59,								"B3" },
	{ 60,								"C4" },
	{ 61,								"C#4" },
	{ 62,								"D4" },
	{ 63,								"D#4" },
	{ 64,								"E4" },
	{ 65,								"F4" },
	{ 66,								"F#4" },
	{ 67,								"G4" },
	{ 68,								"G#4" },
	{ 69,								"A4" },
	{ 70,								"A#4" },
	{ 71,								"B4" },
	{ 72,								"C5" },
	{ 73,								"C#5" },
	{ 74,								"D5" },
	{ 75,								"D#5" },
	{ 76,								"E5" },
	{ 77,								"F5" },
	{ 78,								"F#5" },
	{ 79,								"G5" },
	{ 80,								"G#5" },
	{ 81,								"A5" },
	{ 82,								"A#5" },
	{ 83,								"B5" },
	{ 84,								"C6" },
	{ 85,								"C#6" },
	{ 86,								"D6" },
	{ 87,								"D#6" },
	{ 88,								"E6" },
	{ 89,								"F6" },
	{ 90,								"F#6" },
	{ 91,								"G6" },
	{ 92,								"G#6" },
	{ 93,								"A6" },
	{ 94,								"A#6" },
	{ 95,								"B6" },
	{ 96,								"C7" },
	{ 97,								"C#7" },
	{ 98,								"D7" },
	{ 99,								"D#7" },
	{ 100,								"E7" },
	{ 101,								"F7" },
	{ 102,								"F#7" },
	{ 103,								"G7" },
	{ 104,								"G#7" },
	{ 105,								"A7" },
	{ 106,								"A#7" },
	{ 107,								"B7" },
	{ 108,								"C8" },
	{ 109,								"C#8" },
	{ 110,								"D8" },
	{ 111,								"D#8" },
	{ 112,								"E8" },
	{ 113,								"F8" },
	{ 114,								"F#8" },
	{ 115,								"G8" },
	{ 116,								"G#8" },
	{ 117,								"A8" },
	{ 118,								"A#8" },
	{ 119,								"B8" },
	{ 120,								"C9" },
	{ 121,								"C#9" },
	{ 122,								"D9" },
	{ 123,								"D#9" },
	{ 124,								"E9" },
	{ 125,								"F9" },
	{ 126,								"F#9" },
	{ 127,								"G9" },
};

END_MIDI_NAMESPACE
