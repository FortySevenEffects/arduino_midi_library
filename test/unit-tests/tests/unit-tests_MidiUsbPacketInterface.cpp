#include "unit-tests.h"
#include <src/midi_UsbDefs.h>
#include <src/midi_RingBuffer.h>
#include <src/midi_UsbPacketInterface.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using Buffer = midi::RingBuffer<uint8_t, 8>;

TEST(MidiUsbPacketInterfaceTx, EmptyBufferShouldNotComposeAPacket)
{
    midiEventPacket_t packet;
    Buffer buffer;

    const bool result = midi::composeTxPacket(buffer, packet);
    EXPECT_FALSE(result) << "Empty buffer should not compose a packet";
}

TEST(MidiUsbPacketInterfaceTx, IncompleteDataShouldNotComposeAPacket)
{
    midiEventPacket_t packet;
    Buffer buffer;
    bool result = false;

    buffer.write(0x81);
    result = midi::composeTxPacket(buffer, packet);
    EXPECT_FALSE(result) << "Insufficient data should not compose a packet";
    EXPECT_EQ(1, buffer.getLength()) << "Partial data should not be read out of the buffer";

    buffer.write(0x12);
    buffer.write(0x42);
    result = midi::composeTxPacket(buffer, packet);
    EXPECT_TRUE(result) << "Complete data should compose a packet";
    EXPECT_EQ(0, buffer.getLength()) << "Complete packet data should be read out of the buffer";
    EXPECT_EQ(midi::CodeIndexNumbers::noteOff, packet.header);
    EXPECT_EQ(0x81, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
}

// Channel Voice Messages --

TEST(MidiUsbPacketInterfaceTx, NoteOff)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0x80);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::noteOff, packet.header);
    EXPECT_EQ(0x80, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, NoteOn)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0x91);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::noteOn, packet.header);
    EXPECT_EQ(0x91, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, PolyPressure)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0xA2);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::polyPressure, packet.header);
    EXPECT_EQ(0xA2, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, ControlChange)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0xB3);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::controlChange, packet.header);
    EXPECT_EQ(0xB3, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, ProgramChange)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0xC4);
    buffer.write(0x12);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::programChange, packet.header);
    EXPECT_EQ(0xC4, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, ChannelPressure)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0xD5);
    buffer.write(0x12);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::channelPressure, packet.header);
    EXPECT_EQ(0xD5, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, PitchBend)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0xE6);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::pitchBend, packet.header);
    EXPECT_EQ(0xE6, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

// System Real Time Messages --

TEST(MidiUsbPacketInterfaceTx, Clock)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::Clock);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::Clock, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, Start)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::Start);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::Start, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, Stop)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::Stop);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::Stop, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, Continue)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::Continue);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::Continue, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, ActiveSensing)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::ActiveSensing);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::ActiveSensing, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SystemReset)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::SystemReset);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::singleByte, packet.header);
    EXPECT_EQ(midi::SystemReset, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

// System Common Messages --

TEST(MidiUsbPacketInterfaceTx, TuneRequest)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::TuneRequest);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::systemCommon1Byte, packet.header);
    EXPECT_EQ(midi::TuneRequest, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SongSelect)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::SongSelect);
    buffer.write(0x12);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::systemCommon2Bytes, packet.header);
    EXPECT_EQ(midi::SongSelect, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SongPosition)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::SongPosition);
    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::systemCommon3Bytes, packet.header);
    EXPECT_EQ(midi::SongPosition, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x42, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, TimeCodeQuarterFrame)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(midi::TimeCodeQuarterFrame);
    buffer.write(0x12);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(midi::CodeIndexNumbers::systemCommon2Bytes, packet.header);
    EXPECT_EQ(midi::TimeCodeQuarterFrame, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

// -----------------------------------------------------------------------------

TEST(MidiUsbPacketInterfaceRx, PacketParsing)
{

}

END_UNNAMED_NAMESPACE
