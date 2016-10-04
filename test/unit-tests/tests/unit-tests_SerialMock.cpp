#include "unit-tests.h"
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

USING_NAMESPACE_TEST_MOCKS
using namespace testing;

TEST(RingBuffer, initialState)
{
    typedef RingBuffer<uint8, 32> Buffer;
    Buffer buffer;
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
    buffer.clear();
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
}

TEST(RingBuffer, uint8)
{
    typedef RingBuffer<uint8, 8> Buffer;
    Buffer buffer;

    buffer.write(42);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.isEmpty(),   false);

    const uint8 read = buffer.read();
    EXPECT_EQ(read, 42);
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);

    const uint8 data[] = "Hello, World!";
    buffer.write(data, 13);
    EXPECT_EQ(buffer.getLength(), 5); // 13 % 8
    EXPECT_EQ(buffer.isEmpty(), false);

    uint8 output[8] = { 0 };
    buffer.read(output, 8);
    const uint8 expected[8] = {
        'o', 'r', 'l', 'd', '!', ',', ' ', 'W',
    };
    EXPECT_THAT(output, ContainerEq(expected));

    buffer.clear();
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
}

TEST(RingBuffer, uint32)
{
    typedef RingBuffer<uint32_t, 32> Buffer;
    Buffer buffer;
    buffer.write(42);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.isEmpty(),   false);
    const uint8 read = buffer.read();
    EXPECT_EQ(read, 42);
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
}

END_UNNAMED_NAMESPACE
