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
    const int bufferLength = inBuffer.getLength();
    const byte status = inBuffer.peek();
    const byte cin = midi::CodeIndexNumbers::fromStatus(status);
    const byte messageLength = midi::CodeIndexNumbers::getSize(cin);

    if (status == 0xf0)
    {
        // Start of SysEx, check if it can end in one go.
        if (bufferLength == 2 && inBuffer.peek(1) == 0xf7)
        {
            outPacket.header = midi::CodeIndexNumbers::sysExEnds2Bytes;
            outPacket.byte1 = status;
            outPacket.byte2 = 0xf7;
            outPacket.byte3 = 0x00;
            inBuffer.pop(2);
            return true;
        }
        if (bufferLength >= 3 && inBuffer.peek(2) == 0xf7)
        {
            outPacket.header = midi::CodeIndexNumbers::sysExEnds3Bytes;
            outPacket.byte1 = status;
            outPacket.byte2 = inBuffer.peek(1);
            outPacket.byte3 = 0xf7;
            inBuffer.pop(3);
            return true;
        }
    }

    if ((status & 0x80) == 0x00)
    {
        // First byte is data, consider it's part of a running SysEx message.
        // We look for the SysEx end byte in the next 2 bytes
        // At this point, bufferLength should be 2 or more to continue.
        if (bufferLength == 1)
        {
            return false; // Not enough data
        }
        if (bufferLength == 2)
        {
            const bool isSysExEnd = inBuffer.peek(1) == 0xf7;
            if (!isSysExEnd)
            {
                return false; // Not enough data (eg: 0x12 0x42)
            }
            // eg: 0x42 0xf7
            outPacket.header = midi::CodeIndexNumbers::sysExEnds2Bytes;
            outPacket.byte1 = status;
            outPacket.byte2 = inBuffer.peek(1);
            outPacket.byte3 = 0x00;
            inBuffer.pop(2);
            return true;
        }
        else
        {
            // bufferLength > 2
            const byte byte3 = inBuffer.peek(2);
            outPacket.header = byte3 == 0xf7
                ? midi::CodeIndexNumbers::sysExEnds3Bytes
                : midi::CodeIndexNumbers::sysExContinue;
            outPacket.byte1 = status;
            outPacket.byte2 = inBuffer.peek(1);
            outPacket.byte3 = byte3;
            inBuffer.pop(3);
            return true;
        }
    }

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
}

template<typename Buffer>
void serialiseRxPacket(const midiEventPacket_t& inPacket, Buffer& outBuffer)
{
    const byte cin = inPacket.header & 0x0f;
    const byte messageLength = midi::CodeIndexNumbers::getSize(cin);
    switch (messageLength)
    {
        case 1:
            outBuffer.write(inPacket.byte1);
            return;
        case 2:
            outBuffer.write(inPacket.byte1);
            outBuffer.write(inPacket.byte2);
            return;
        case 3:
            outBuffer.write(inPacket.byte1);
            outBuffer.write(inPacket.byte2);
            outBuffer.write(inPacket.byte3);
            return;
        case 0:
        default:
            // Invalid or ignored messages, don't serialise.
            return;
    }
}

END_MIDI_NAMESPACE
