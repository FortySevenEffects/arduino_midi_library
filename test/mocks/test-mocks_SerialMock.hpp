#pragma once

BEGIN_TEST_MOCKS_NAMESPACE

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
DataType RingBuffer<DataType, Size>::peek() const
{
    return *mReadHead;
}

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

// =============================================================================

template<int BufferSize>
SerialMock<BufferSize>::SerialMock()
{
}

template<int BufferSize>
SerialMock<BufferSize>::~SerialMock()
{
}

// -----------------------------------------------------------------------------

template<int BufferSize>
void SerialMock<BufferSize>::begin(int inBaudrate)
{
    mBaudrate = inBaudrate;
    mTxBuffer.clear();
    mRxBuffer.clear();
}

template<int BufferSize>
int SerialMock<BufferSize>::available() const
{
    return mRxBuffer.getLength();
}

template<int BufferSize>
void SerialMock<BufferSize>::write(uint8 inData)
{
    mTxBuffer.write(inData);
}

template<int BufferSize>
uint8 SerialMock<BufferSize>::read()
{
    return mRxBuffer.read();
}

// -----------------------------------------------------------------------------

template<int BufferSize>
void SerialMock<BufferSize>::moveTxToRx()
{
    mRxBuffer.clear();
    const int size = mTxBuffer.getSize();
    for (int i = 0; i < size; ++i)
    {
        mRxBuffer.write(mTxBuffer.read());
    }
}

END_TEST_MOCKS_NAMESPACE
