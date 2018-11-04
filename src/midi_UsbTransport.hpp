/*!
 *  @file       midi_UsbTransport.hpp
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

BEGIN_MIDI_NAMESPACE

template<unsigned BufferSize>
inline UsbTransport<BufferSize>::UsbTransport()
{

}

template<unsigned BufferSize>
inline UsbTransport<BufferSize>::~UsbTransport()
{

}

// -----------------------------------------------------------------------------

template<unsigned BufferSize>
inline void UsbTransport<BufferSize>::begin(unsigned inBaudrate)
{
    mTxBuffer.clear();
    mRxBuffer.clear();
}

template<unsigned BufferSize>
inline unsigned UsbTransport<BufferSize>::available()
{
    pollUsbMidi();
    return mRxBuffer.getLength();
}

template<unsigned BufferSize>
inline byte UsbTransport<BufferSize>::read()
{
    return mRxBuffer.read();
}

template<unsigned BufferSize>
inline void UsbTransport<BufferSize>::write(byte inData)
{
    mTxBuffer.write(inData);
    recomposeAndSendTxPackets();
}

// -----------------------------------------------------------------------------

template<unsigned BufferSize>
inline void UsbTransport<BufferSize>::pollUsbMidi()
{
    midiEventPacket_t packet = MidiUSB.read();
    while (packet.header != 0)
    {
        serialiseRxPacket(packet, mRxBuffer);
        packet = MidiUSB.read();
    }
}

template<unsigned BufferSize>
inline void UsbTransport<BufferSize>::recomposeAndSendTxPackets()
{
    midiEventPacket_t packet;
    bool sent = false;
    while (composeTxPacket(mTxBuffer, packet))
    {
        MidiUSB.sendMIDI(packet);
        sent = true;
    }
    if (sent)
    {
        MidiUSB.flush();
    }
}

END_MIDI_NAMESPACE
