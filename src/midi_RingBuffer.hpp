/*!
 *  @file       midi_RingBuffer.hpp
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for Arduino - Ring Buffer
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

namespace {

    template<int N>
    struct isPowerOfTwo
    {
        static const bool value = N && !(N & (N - 1));
    };

}

// --

BEGIN_MIDI_NAMESPACE

template<typename DataType, int Size>
RingBuffer<DataType, Size>::RingBuffer()
    : mLength(0)
    , mWriteHead(0)
    , mReadHead(0)
{
    static_assert(isPowerOfTwo<Size>::value, "Size must be a power of two.");
    memset(mData, DataType(0), Size * sizeof(DataType));
}

template<typename DataType, int Size>
RingBuffer<DataType, Size>::~RingBuffer()
{
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
inline int RingBuffer<DataType, Size>::getLength() const
{
    return mLength;
}

template<typename DataType, int Size>
inline bool RingBuffer<DataType, Size>::isEmpty() const
{
    return mLength == 0;
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
void RingBuffer<DataType, Size>::write(DataType inData)
{
    mData[mWriteHead] = inData;
    mWriteHead = (mWriteHead + 1) & sMask;
    mLength++;
    if (mLength > Size) {
        mLength = Size;
        mReadHead = (mReadHead + 1) & sMask;
    }
}

template<typename DataType, int Size>
void RingBuffer<DataType, Size>::write(const DataType* inData, int inSize)
{
    for (int i = 0; i < inSize; ++i)
    {
        write(inData[i]);
    }
}

template<typename DataType, int Size>
void RingBuffer<DataType, Size>::clear()
{
    memset(mData, DataType(0), Size * sizeof(DataType));
    mReadHead  = 0;
    mWriteHead = 0;
    mLength = 0;
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
DataType RingBuffer<DataType, Size>::read()
{
    mLength--;
    if (mLength < 0) {
        mLength = 0;
        return 0;
    }
    const DataType data = mData[mReadHead];
    mReadHead = (mReadHead + 1) & sMask;
    return data;
}

template<typename DataType, int Size>
void RingBuffer<DataType, Size>::read(DataType* outData, int inSize)
{
    for (int i = 0; i < inSize; ++i)
    {
        outData[i] = read();
    }
}

END_MIDI_NAMESPACE
