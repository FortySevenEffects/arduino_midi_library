#include "unit-tests.h"
#include <src/MIDI.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using namespace testing;

TEST(SysExCodec, Encoder)
{
    // ASCII content
    {
        const byte input[] = "Hello, World!";
        byte buffer[32];
        memset(buffer, 0, 32 * sizeof(byte));
        const unsigned encodedSize = midi::encodeSysEx(input, buffer, 13);
        EXPECT_EQ(encodedSize, 15);
        const byte expected[32] = {
            0, 'H', 'e', 'l', 'l', 'o', ',', ' ',
            0, 'W', 'o', 'r', 'l', 'd', '!', 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        };
        EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
        EXPECT_THAT(buffer, ContainerEq(expected));
    }
    // Non-ASCII content
    {
        const byte input[] = {
            182, 236, 167, 177, 61, 91, 120,    // 01111000 -> 120
            107, 94, 209, 87, 94                // 000100xx -> 16
        };
        byte buffer[32];
        memset(buffer, 0, 32 * sizeof(byte));
        const unsigned encodedSize = midi::encodeSysEx(input, buffer, 12);
        EXPECT_EQ(encodedSize, 14);
        const byte expected[32] = {
        //  MSB    Data
            120,   54, 108, 39, 49, 61, 91, 120,
            16,    107, 94, 81, 87, 94, 0,  0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        };
        EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
        EXPECT_THAT(buffer, ContainerEq(expected));
    }
}

TEST(SysExCodec, Decoder)
{
    // ASCII content
    {
        const byte input[] = {
            0, 'H', 'e', 'l', 'l', 'o', ',', ' ',
            0, 'W', 'o', 'r', 'l', 'd', '!',
        };
        byte buffer[32];
        memset(buffer, 0, 32 * sizeof(byte));
        const unsigned decodedSize = midi::decodeSysEx(input, buffer, 15);
        EXPECT_EQ(decodedSize, 13);
        const byte expected[32] = {
            'H', 'e', 'l', 'l', 'o', ',', ' ', 'W',
            'o', 'r', 'l', 'd', '!', 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        };
        EXPECT_THAT(buffer, Each(Le(0x7f))); // All elements are <= 127
        EXPECT_THAT(buffer, ContainerEq(expected));
    }
    // Non-ASCII content
    {
        const byte input[] = {
        //  MSB    Data
            120,   54, 108, 39, 49, 61, 91, 120,
            16,    107, 94, 81, 87, 94,
        };
        byte buffer[32];
        memset(buffer, 0, 32 * sizeof(byte));
        const unsigned encodedSize = midi::decodeSysEx(input, buffer, 14);
        EXPECT_EQ(encodedSize, 12);
        const byte expected[32] = {
            182, 236, 167, 177, 61, 91, 120,
            107, 94, 209, 87, 94, 0, 0,
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0
        };
        EXPECT_THAT(input,  Each(Le(0x7f))); // All elements are <= 127
        EXPECT_THAT(buffer, ContainerEq(expected));
    }
}

TEST(SysExCodec, Codec)
{

}

END_UNNAMED_NAMESPACE
