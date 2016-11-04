/*!
 *  @file       midi_UsbTransport.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Transport layer for USB MIDI
 *  @author     Francois Best
 *  @date       10/10/2016
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

#include "midi_Defs.h"
#include "midi_RingBuffer.h"
#include <MIDIUSB.h>

BEGIN_MIDI_NAMESPACE

template<unsigned BuffersSize>
class UsbTransport
{
public:
    inline UsbTransport();
    inline ~UsbTransport();

public: // Serial / Stream API required for template compatibility
    inline void begin(unsigned inBaudrate);
    inline unsigned available();
    inline byte read();
    inline void write(byte inData);

private:
    inline bool pollUsbMidi();
    inline void recomposeAndSendTxPackets();
    inline void resetTx();
    static inline byte encodePacketHeader(StatusByte inStatusByte);
    static inline int getPacketLength(const midiEventPacket_t& inPacket);

private:
    typedef RingBuffer<byte, BuffersSize> Buffer;
    Buffer mTxBuffer;
    Buffer mRxBuffer;

    union TxPacket
    {
        byte mDataArray[4];
        midiEventPacket_t mPacket;
    };
    TxPacket mCurrentTxPacket;
    int mCurrentTxPacketByteIndex;
};

END_MIDI_NAMESPACE

#include "midi_UsbTransport.hpp"
