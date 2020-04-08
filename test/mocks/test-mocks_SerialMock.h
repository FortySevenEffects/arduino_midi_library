#pragma once

#include "test-mocks.h"
#include <inttypes.h>

BEGIN_TEST_MOCKS_NAMESPACE

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
    DataType peek() const;
    DataType read();
    void read(DataType* outData, int inSize);

private:
    DataType mData[Size];
    DataType* mWriteHead;
    DataType* mReadHead;
};

template<int BufferSize>
class SerialMock
{
public:
     SerialMock();
    ~SerialMock();

public: // Arduino Serial API
    void begin(int inBaudrate);
    int available() const;
    void write(uint8_t inData);
    uint8_t read();

public: // Test Helpers API
    void moveTxToRx(); // Simulate loopback

public:
    typedef RingBuffer<uint8_t, BufferSize> Buffer;
    Buffer mTxBuffer;
    Buffer mRxBuffer;
    int mBaudrate;
};

END_TEST_MOCKS_NAMESPACE

#include "test-mocks_SerialMock.hpp"
