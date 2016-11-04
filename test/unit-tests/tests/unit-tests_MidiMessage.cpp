#include "unit-tests.h"
#include <src/midi_Message.h>

BEGIN_MIDI_NAMESPACE

// Declare references:
// http://stackoverflow.com/questions/4891067/weird-undefined-symbols-of-static-constants-inside-a-struct-class

template<unsigned Size>
const unsigned Message<Size>::sSysExMaxSize;

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

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
    EXPECT_EQ(message.getSysExSize(), unsigned(0));
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
        ASSERT_EQ(Message::sSysExMaxSize, unsigned(32));
        Message message = Message();

        const unsigned sizeUnder = 20;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 64;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), unsigned(32));
    }
    // Medium message
    {
        typedef midi::Message<256> Message;
        ASSERT_EQ(Message::sSysExMaxSize, unsigned(256));
        Message message = Message();

        const unsigned sizeUnder = 200;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 300;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), unsigned(256));
    }
    // Large message
    {
        typedef midi::Message<1024> Message;
        ASSERT_EQ(Message::sSysExMaxSize, unsigned(1024));
        Message message = Message();

        const unsigned sizeUnder = 1000;
        setSysExSize(message, sizeUnder);
        ASSERT_EQ(message.getSysExSize(), sizeUnder);

        const unsigned sizeOver  = 2000;
        setSysExSize(message, sizeOver);
        ASSERT_EQ(message.getSysExSize(), unsigned(1024));
    }
}

END_UNNAMED_NAMESPACE
