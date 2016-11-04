#include "unit-tests.h"
#include <src/midi_UsbDefs.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

TEST(MidiUsb, codeIndexNumberSizes)
{
    typedef midi::CodeIndexNumbers CIN;
    EXPECT_EQ(CIN::getSize(CIN::reserved),              0);
    EXPECT_EQ(CIN::getSize(CIN::misc),                  0);
    EXPECT_EQ(CIN::getSize(CIN::cableEvent),            0);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon2Bytes),    2);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon3Bytes),    3);
    EXPECT_EQ(CIN::getSize(CIN::sysExStart),            3);
    EXPECT_EQ(CIN::getSize(CIN::sysExContinue),         3);
    EXPECT_EQ(CIN::getSize(CIN::systemCommon1Byte),     1);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds1Byte),        1);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds2Bytes),       2);
    EXPECT_EQ(CIN::getSize(CIN::sysExEnds3Bytes),       3);
    EXPECT_EQ(CIN::getSize(CIN::noteOff),               3);
    EXPECT_EQ(CIN::getSize(CIN::noteOn),                3);
    EXPECT_EQ(CIN::getSize(CIN::polyPressure),          3);
    EXPECT_EQ(CIN::getSize(CIN::controlChange),         3);
    EXPECT_EQ(CIN::getSize(CIN::programChange),         2);
    EXPECT_EQ(CIN::getSize(CIN::channelPressure),       2);
    EXPECT_EQ(CIN::getSize(CIN::pitchBend),             3);
    EXPECT_EQ(CIN::getSize(CIN::singleByte),            1);
}

TEST(MidiUsb, UsbMidiEventPacket)
{
    midi::UsbMidiEventPacket packet;
    EXPECT_EQ(packet.mData[0],              0);
    EXPECT_EQ(packet.mData[1],              0);
    EXPECT_EQ(packet.mData[2],              0);
    EXPECT_EQ(packet.mData[3],              0);
    EXPECT_EQ(packet.getCableNumber(),      0);
    EXPECT_EQ(packet.getCodeIndexNumber(),  0);

    packet.setHeader(12, 7);
    EXPECT_EQ(packet.mData[0],              0xc7);
    EXPECT_EQ(packet.getCableNumber(),      12);
    EXPECT_EQ(packet.getCodeIndexNumber(),  7);

    const byte midiData[3] = { 12, 42, 47 };
    packet.setMidiData(midiData);
    EXPECT_EQ(packet.mData[0], 0xc7);
    EXPECT_EQ(packet.mData[1], 12);
    EXPECT_EQ(packet.mData[2], 42);
    EXPECT_EQ(packet.mData[3], 47);

    const byte fullData[4] = { 12, 34, 56, 78 };
    packet = fullData;
    EXPECT_EQ(packet.mData[0], 12);
    EXPECT_EQ(packet.mData[1], 34);
    EXPECT_EQ(packet.mData[2], 56);
    EXPECT_EQ(packet.mData[3], 78);

    const byte* midiDataConst = packet.getMidiData();
    EXPECT_EQ(midiDataConst[0], 34);
    EXPECT_EQ(midiDataConst[1], 56);
    EXPECT_EQ(midiDataConst[2], 78);
    byte* midiDataMutable = packet.getMidiData();
    EXPECT_EQ(midiDataMutable[0], 34);
    EXPECT_EQ(midiDataMutable[1], 56);
    EXPECT_EQ(midiDataMutable[2], 78);
}

END_UNNAMED_NAMESPACE
