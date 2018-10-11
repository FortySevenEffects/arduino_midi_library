#include "unit-tests.h"
#include <src/midi_RingBuffer.h>

BEGIN_UNNAMED_NAMESPACE
using namespace testing;
using Buffer = midi::RingBuffer<uint8_t, 8>;

TEST(RingBuffer, writeScalar)
{
    Buffer buffer;
    EXPECT_EQ(buffer.isEmpty(), true);
    EXPECT_EQ(buffer.getLength(), 0);
    buffer.write(42);
    buffer.write(47);
    EXPECT_EQ(buffer.isEmpty(), false);
    EXPECT_EQ(buffer.getLength(), 2);
}

TEST(RingBuffer, readScalar)
{
    Buffer buffer;
    buffer.write(42);
    EXPECT_EQ(buffer.getLength(), 1);
    buffer.write(47);
    EXPECT_EQ(buffer.getLength(), 2);
    EXPECT_EQ(buffer.read(), 42);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.read(), 47);
    EXPECT_EQ(buffer.isEmpty(), true);
    EXPECT_EQ(buffer.getLength(), 0);
}

TEST(RingBuffer, clear)
{
    Buffer buffer;
    buffer.write(42);
    buffer.write(47);
    buffer.clear();
    EXPECT_EQ(buffer.isEmpty(), true);
    EXPECT_EQ(buffer.getLength(), 0);
}

TEST(RingBuffer, writeArray)
{
    Buffer buffer;
    const uint8_t input[4] = {
        1, 2, 3, 4
    };
    buffer.write(input, 4);
    EXPECT_EQ(buffer.isEmpty(), false);
    EXPECT_EQ(buffer.getLength(), 4);
}

TEST(RingBuffer, writeOverflow)
{
    Buffer buffer;

    buffer.write(1);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(2);
    EXPECT_EQ(buffer.getLength(), 2);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(3);
    EXPECT_EQ(buffer.getLength(), 3);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(4);
    EXPECT_EQ(buffer.getLength(), 4);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(5);
    EXPECT_EQ(buffer.getLength(), 5);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(6);
    EXPECT_EQ(buffer.getLength(), 6);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(7);
    EXPECT_EQ(buffer.getLength(), 7);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(8);
    EXPECT_EQ(buffer.getLength(), 8);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(9);
    EXPECT_EQ(buffer.getLength(), 8);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(10);
    EXPECT_EQ(buffer.getLength(), 8);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(11);
    EXPECT_EQ(buffer.getLength(), 8);
    EXPECT_EQ(buffer.isEmpty(), false);
    buffer.write(12);
    EXPECT_EQ(buffer.getLength(), 8);
    EXPECT_EQ(buffer.isEmpty(), false);
}

TEST(RingBuffer, readOverflow)
{
    Buffer buffer;

    buffer.write(1);
    buffer.write(2);
    buffer.write(3);
    buffer.write(4);
    buffer.write(5);
    buffer.write(6);
    buffer.write(7);
    buffer.write(8);
    buffer.write(9);
    buffer.write(10);
    buffer.write(11);
    buffer.write(12);
    EXPECT_EQ(buffer.read(), 5);
    EXPECT_EQ(buffer.getLength(), 7);
    EXPECT_EQ(buffer.read(), 6);
    EXPECT_EQ(buffer.getLength(), 6);
    EXPECT_EQ(buffer.read(), 7);
    EXPECT_EQ(buffer.getLength(), 5);
    EXPECT_EQ(buffer.read(), 8);
    EXPECT_EQ(buffer.getLength(), 4);
    EXPECT_EQ(buffer.read(), 9);
    EXPECT_EQ(buffer.getLength(), 3);
    EXPECT_EQ(buffer.read(), 10);
    EXPECT_EQ(buffer.getLength(), 2);
    EXPECT_EQ(buffer.read(), 11);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.read(), 12);
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.read(), 0);
    EXPECT_EQ(buffer.getLength(), 0);
}

TEST(RingBuffer, writeArrayOverflow)
{
    Buffer buffer;
    const uint8_t input[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    };
    buffer.write(input, 12);
    EXPECT_EQ(buffer.isEmpty(), false);
    EXPECT_EQ(buffer.getLength(), 8); // Max size
}

TEST(RingBuffer, readArray)
{
    Buffer buffer;
    const uint8_t input[4] = {
        1, 2, 3, 4
    };
    uint8_t output[4] = { 0 };
    buffer.write(input, 4);
    buffer.read(output, 4);
    EXPECT_THAT(output, ContainerEq(input));
}

TEST(RingBuffer, readArrayOverflow)
{
    Buffer buffer;
    const uint8_t input[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    };
    const uint8_t expected[8] = {
        5, 6, 7, 8, 9, 10, 11, 12,
    };
    uint8_t output[8] = { 0 };

    buffer.write(input, 12);
    buffer.read(output, 8);
    EXPECT_THAT(output, ContainerEq(expected));
    EXPECT_EQ(buffer.isEmpty(), true);
    EXPECT_EQ(buffer.getLength(), 0);
}

END_UNNAMED_NAMESPACE

