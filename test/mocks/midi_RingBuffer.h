/*!
 *  @file       midi_RingBuffer.h
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

#include "midi_Namespace.h"

BEGIN_MIDI_NAMESPACE

template<typename DataType, int Size>
class RingBuffer
{
public:
     RingBuffer();
    ~RingBuffer();

public:
    int getLength() const;
    bool isEmpty() const;

public:
    void write(DataType inData);
    void write(const DataType* inData, int inSize);
    void clear();

public:
    DataType read();
    void read(DataType* outData, int inSize);

private:
    DataType mData[Size];
    DataType* mWriteHead;
    DataType* mReadHead;
};

END_MIDI_NAMESPACE

#include "midi_RingBuffer.hpp"
