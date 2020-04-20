#include "unit-tests.h"
#include <src/MIDI.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using namespace testing;

TEST(SysExCodec, EncoderAscii)
{
    const byte input[] = "Hello, World!";
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer, 13);
    EXPECT_EQ(encodedSize, unsigned(15));
    const byte expected[16] = {
        0, 'H', 'e', 'l', 'l', 'o', ',', ' ',
        0, 'W', 'o', 'r', 'l', 'd', '!', 0,
    };
    EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

TEST(SysExCodec, EncoderNonAscii)
{
    const byte input[] = {
        182, 236, 167, 177, 61, 91, 120,    // 01111000 -> 120
        107, 94, 209, 87, 94                // 000100xx -> 16
    };
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer, 12);
    EXPECT_EQ(encodedSize, unsigned(14));
    const byte expected[16] = {
    //  MSB    Data
        120,   54, 108, 39, 49, 61, 91, 120,
        16,    107, 94, 81, 87, 94, 0,  0,
    };
    EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

TEST(SysExCodec, EncoderNonAsciiFlipHeader)
{
    const byte input[] = {
        182, 236, 167, 177, 61, 91, 120,    // 00011111 -> 15
        107, 94, 209, 87, 94                // 0xx00100 -> 4
    };
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer, 12, true);
    EXPECT_EQ(encodedSize, unsigned(14));
    const byte expected[16] = {
    //  MSB    Data
        15,    54, 108, 39, 49, 61, 91, 120,
        4,     107, 94, 81, 87, 94, 0,  0,
    };
    EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

// -----------------------------------------------------------------------------

TEST(SysExCodec, DecoderAscii)
{
    const byte input[] = {
        0, 'H', 'e', 'l', 'l', 'o', ',', ' ',
        0, 'W', 'o', 'r', 'l', 'd', '!',
    };
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned decodedSize = midi::decodeSysEx(input, buffer, 15);
    EXPECT_EQ(decodedSize, unsigned(13));
    const byte expected[16] = {
        'H', 'e', 'l', 'l', 'o', ',', ' ', 'W',
        'o', 'r', 'l', 'd', '!', 0, 0, 0,
    };
    EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

    // Non-ASCII content
TEST(SysExCodec, DecoderNonAscii)
{
    const byte input[] = {
    //  MSB    Data
        120,   54, 108, 39, 49, 61, 91, 120,
        16,    107, 94, 81, 87, 94,
    };
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::decodeSysEx(input, buffer, 14);
    EXPECT_EQ(encodedSize, unsigned(12));
    const byte expected[16] = {
        182, 236, 167, 177, 61, 91, 120,
        107, 94, 209, 87, 94, 0, 0,
        0, 0,
    };
    EXPECT_THAT(input,  Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

TEST(SysExCodec, DecoderNonAsciiFlipHeader)
{
    const byte input[] = {
    //  MSB    Data
        15,    54, 108, 39, 49, 61, 91, 120,
        4,     107, 94, 81, 87, 94,
    };
    byte buffer[16];
    memset(buffer, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::decodeSysEx(input, buffer, 14, true);
    EXPECT_EQ(encodedSize, unsigned(12));
    const byte expected[16] = {
        182, 236, 167, 177, 61, 91, 120,
        107, 94, 209, 87, 94, 0, 0,
        0, 0,
    };
    EXPECT_THAT(input,  Each(Le(0x7f))); // All elements are <= 127
    EXPECT_THAT(buffer, ContainerEq(expected));
}

// -----------------------------------------------------------------------------

TEST(SysExCodec, CodecAscii)
{
    const byte input[] = "Hello, World!";
    byte buffer1[16];
    byte buffer2[16];
    memset(buffer1, 0, 16 * sizeof(byte));
    memset(buffer2, 0, 16 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer1, 13);
    EXPECT_EQ(encodedSize, unsigned(15));
    const unsigned decodedSize = midi::decodeSysEx(buffer1, buffer2, encodedSize);
    EXPECT_EQ(decodedSize, unsigned(13));
    EXPECT_STREQ(reinterpret_cast<const char*>(buffer2),
                 reinterpret_cast<const char*>(input));
}

TEST(SysExCodec, CodecNonAscii)
{
    const byte input[] = {
    //  MSB    Data
        182, 236, 167, 177, 61, 91, 120,
        107, 94, 209, 87, 94
    };
    byte buffer1[14];
    byte buffer2[12];
    memset(buffer1, 0, 14 * sizeof(byte));
    memset(buffer2, 0, 12 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer1, 12);
    EXPECT_EQ(encodedSize, unsigned(14));
    const unsigned decodedSize = midi::decodeSysEx(buffer1, buffer2, encodedSize);
    EXPECT_EQ(decodedSize, unsigned(12));
    EXPECT_THAT(buffer2, ContainerEq(input));
}

TEST(SysExCodec, CodecNonAsciiFlipHeader)
{
    const byte input[] = {
    //  MSB    Data
        182, 236, 167, 177, 61, 91, 120,
        107, 94, 209, 87, 94
    };
    byte buffer1[14];
    byte buffer2[12];
    memset(buffer1, 0, 14 * sizeof(byte));
    memset(buffer2, 0, 12 * sizeof(byte));
    const unsigned encodedSize = midi::encodeSysEx(input, buffer1, 12, true);
    EXPECT_EQ(encodedSize, unsigned(14));
    const unsigned decodedSize = midi::decodeSysEx(buffer1, buffer2, encodedSize, true);
    EXPECT_EQ(decodedSize, unsigned(12));
    EXPECT_THAT(buffer2, ContainerEq(input));
}

END_UNNAMED_NAMESPACE
