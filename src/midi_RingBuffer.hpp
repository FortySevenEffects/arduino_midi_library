#pragma once

BEGIN_MIDI_NAMESPACE

template<typename DataType, int Size>
RingBuffer<DataType, Size>::RingBuffer()
    : mWriteHead(mData)
    , mReadHead(mData)
{
    memset(mData, DataType(0), Size * sizeof(DataType));
}

template<typename DataType, int Size>
RingBuffer<DataType, Size>::~RingBuffer()
{
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
int RingBuffer<DataType, Size>::getLength() const
{
    if (mReadHead == mWriteHead)
    {
        return 0;
    }
    else if (mWriteHead > mReadHead)
    {
        return int(mWriteHead - mReadHead);
    }
    else
    {
        return int(mWriteHead - mData) + Size - int(mReadHead - mData);
    }
}

template<typename DataType, int Size>
bool RingBuffer<DataType, Size>::isEmpty() const
{
    return mReadHead == mWriteHead;
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
void RingBuffer<DataType, Size>::write(DataType inData)
{
    *mWriteHead++ = inData;
    if (mWriteHead >= mData + Size)
    {
        mWriteHead = mData;
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
    mReadHead  = mData;
    mWriteHead = mData;
}

// -----------------------------------------------------------------------------

template<typename DataType, int Size>
DataType RingBuffer<DataType, Size>::read()
{
    const DataType data = *mReadHead++;
    if (mReadHead >= mData + Size)
    {
        mReadHead = mData;
    }
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
