#pragma once

#include "test-mocks.h"
#include <inttypes.h>
#include <src/midi_RingBuffer.h>

BEGIN_TEST_MOCKS_NAMESPACE

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
    typedef midi::RingBuffer<uint8_t, BufferSize> Buffer;
    Buffer mTxBuffer;
    Buffer mRxBuffer;
    int mBaudrate;
};

END_TEST_MOCKS_NAMESPACE

#include "test-mocks_SerialMock.hpp"
