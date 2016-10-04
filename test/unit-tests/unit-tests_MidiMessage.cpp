#include "unit-tests.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

TEST(MidiMessage, hasTheRightProperties)
{
    typedef midi::Message<42> Message;
    const Message message = Message();
    EXPECT_EQ(message.channel,  0);
    EXPECT_EQ(message.type,     0);
    EXPECT_EQ(message.data1,    0);
    EXPECT_EQ(message.data2,    0);
    EXPECT_EQ(message.valid,    false);
    EXPECT_EQ(message.getSysExSize(), 0);
}

template<typename Message>
inline void setSysExSize(Message& ioMessage, unsigned inSize)
{
    ioMessage.data2 = inSize >> 8;      // MSB
    ioMessage.data1 = inSize & 0xff;    // LSB
}

TEST(MidiMessage, getSysExSize)
{
    // Small message
    {
        typedef midi::Message<32> Message;
        Message message = Message();
        ASSERT_EQ(Message::getSysExMaxSize(), 32);
        ASSERT_EQ(message.getSysExMaxSize(),  32);

        const unsigned sizeUnder = 20;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 64;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), 32);
    }
    // Medium message
    {
        typedef midi::Message<256> Message;
        Message message = Message();
        ASSERT_EQ(Message::getSysExMaxSize(), 256);
        ASSERT_EQ(message.getSysExMaxSize(),  256);

        const unsigned sizeUnder = 200;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 300;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), 256);
    }
    // Large message
    {
        typedef midi::Message<1024> Message;
        Message message = Message();
        ASSERT_EQ(Message::getSysExMaxSize(), 1024);
        ASSERT_EQ(message.getSysExMaxSize(),  1024);

        const unsigned sizeUnder = 1000;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 2000;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), 1024);
    }
}

END_UNNAMED_NAMESPACE
