/*!
 *  @file       midi_UsbDefs.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Definitions
 *  @version    4.2
 *  @author     Francois Best
 *  @date       24/02/11
 *  @license    MIT - Copyright (c) 2016 Francois Best
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

struct UsbMidiEventPacket
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
        byte     mDataArray[4];
    };
};

END_MIDI_NAMESPACE
