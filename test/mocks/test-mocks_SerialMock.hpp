#pragma once

BEGIN_TEST_MOCKS_NAMESPACE

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
void SerialMock<BufferSize>::write(uint8_t inData)
{
    mTxBuffer.write(inData);
}

template<int BufferSize>
uint8_t SerialMock<BufferSize>::read()
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
