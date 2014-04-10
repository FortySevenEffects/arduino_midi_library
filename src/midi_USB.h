/*!
 *  @file       midi_USB.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - USB
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

BEGIN_MIDI_NAMESPACE

struct CodeIndexNumbers
{
    enum
    {
        reserved                = 0x00,
        misc                    = reserved,

        cableEvent              = 0x01,
        systemCommon2Bytes      = 0x02,
        systemCommon3Bytes      = 0x03,

        sysExStart              = 0x04,
        sysExContinue           = sysExStart,

        systemCommon1Byte       = 0x05,
        sysExEnds1Byte          = systemCommon1Byte,

        sysExEnds2Bytes         = 0x06,
        sysExEnds3Bytes         = 0x07,
        noteOn                  = 0x08,
        noteOff                 = 0x09,
        polyKeyPress            = 0x0A,
        controlChange           = 0x0B,
        programChange           = 0x0C,
        channelPressure         = 0x0D,
        pitchBend               = 0x0E,
        singleByte              = 0x0F,
    };

    inline byte getSize(byte inCIN)
    {
        switch (inCIN)
        {
            case noteOn:
            case noteOff:
            case controlChange:
            case pitchBend:
            case polyKeyPress:
            case systemCommon3Bytes:
            case sysExEnds3Bytes:
                return 3;

            case programChange:
            case channelPressure:
            case systemCommon2Bytes:
            case sysExEnds2Bytes:
                return 2;

            case systemCommon1Byte:
            case singleByte:
                return 1;

            default:
                break;
        }
        return 0;
    }
};

// -----------------------------------------------------------------------------

struct UsbMidiEvent
{
    union
    {
        uint32_t    mRawData;
        struct
        {
            byte mCableNumber:4;
            byte mCodeIndexNumber:4;
            byte mMidi[3];
        };
        byte[4]     mDataArray;
    };
};

END_MIDI_NAMESPACE
