/*!
 *  @file       midi_UsbPacketInterface.hpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Transport layer for USB MIDI
 *  @author     Francois Best
 *  @date       2018-11-03
 *  @license    MIT - Copyright (c) 2018 Francois Best
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

BEGIN_MIDI_NAMESPACE

template<typename Buffer>
bool composeTxPacket(Buffer& inBuffer, midiEventPacket_t& outPacket)
{
    if (inBuffer.isEmpty()) {
        return false;
    }
    int bufferLength = inBuffer.getLength();
    const byte status = inBuffer.peek();
    const byte cin = midi::CodeIndexNumbers::fromStatus(status);
    const byte messageLength = midi::CodeIndexNumbers::getSize(cin);

    if (bufferLength < messageLength) {
        return false; // Not enough data in the buffer to compose a full packet.
    }

    outPacket.header = cin;
    outPacket.byte1 = status;
    outPacket.byte2 = messageLength >= 2 ? inBuffer.peek(1) : 0x00;
    outPacket.byte3 = messageLength >= 3 ? inBuffer.peek(2) : 0x00;

    inBuffer.pop(messageLength);
    return true;

    // todo: handle interleaved RealTime messages
    // todo: handle SysEx
}

template<typename Buffer>
bool parseRxPacket(const midiEventPacket_t& /*inPacket*/, Buffer& /*outBuffer*/)
{
    return false;
}

END_MIDI_NAMESPACE
