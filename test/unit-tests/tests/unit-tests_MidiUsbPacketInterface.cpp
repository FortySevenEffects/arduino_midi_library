#include "unit-tests.h"
#include <src/midi_UsbDefs.h>
#include <src/midi_RingBuffer.h>
#include <src/midi_UsbPacketInterface.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using Buffer = midi::RingBuffer<uint8_t, 8>;
using CIN = midi::CodeIndexNumbers;

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
    EXPECT_EQ(CIN::noteOff, packet.header);
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
    EXPECT_EQ(CIN::noteOff, packet.header);
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
    EXPECT_EQ(CIN::noteOn, packet.header);
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
    EXPECT_EQ(CIN::polyPressure, packet.header);
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
    EXPECT_EQ(CIN::controlChange, packet.header);
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
    EXPECT_EQ(CIN::programChange, packet.header);
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
    EXPECT_EQ(CIN::channelPressure, packet.header);
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
    EXPECT_EQ(CIN::pitchBend, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::singleByte, packet.header);
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
    EXPECT_EQ(CIN::systemCommon1Byte, packet.header);
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
    EXPECT_EQ(CIN::systemCommon2Bytes, packet.header);
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
    EXPECT_EQ(CIN::systemCommon3Bytes, packet.header);
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
    EXPECT_EQ(CIN::systemCommon2Bytes, packet.header);
    EXPECT_EQ(midi::TimeCodeQuarterFrame, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

// System Exclusive --

TEST(MidiUsbPacketInterfaceTx, SysExNotEnoughData)
{
    midiEventPacket_t packet;
    Buffer buffer;

    buffer.write(0x12);
    buffer.write(0x42);
    EXPECT_FALSE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(2, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SysExSinglePacket)
{
    midiEventPacket_t packet;
    Buffer buffer;

    // Two-byte SysEx (utterly useless)
    buffer.write(0xf0);
    buffer.write(0xf7);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExEnds2Bytes, packet.header);
    EXPECT_EQ(0xf0, packet.byte1);
    EXPECT_EQ(0xf7, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());

    // Single-data byte SysEx (non-spec conformant ?)
    buffer.write(0xf0);
    buffer.write(0x12);
    buffer.write(0xf7);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExEnds3Bytes, packet.header);
    EXPECT_EQ(0xf0, packet.byte1);
    EXPECT_EQ(0x12, packet.byte2);
    EXPECT_EQ(0xf7, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SysExTwoPackets)
{
    midiEventPacket_t packet;
    Buffer buffer;

    const byte deviceIdentityRequest[6] = {
        0xf0, 0x7e, 0x7f, 0x06, 0x01, 0xf7
    };

    buffer.write(deviceIdentityRequest, 6);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExStart, packet.header);
    EXPECT_EQ(0xf0, packet.byte1);
    EXPECT_EQ(0x7e, packet.byte2);
    EXPECT_EQ(0x7f, packet.byte3);
    EXPECT_EQ(3, buffer.getLength());

    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExEnds3Bytes, packet.header);
    EXPECT_EQ(0x06, packet.byte1);
    EXPECT_EQ(0x01, packet.byte2);
    EXPECT_EQ(0xf7, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SysExMultiplePacketsEndingWith1Byte)
{
    midiEventPacket_t packet;
    Buffer buffer;

    const byte message[7] = {
        0xf0, 0x01, 0x02, 0x03, 0x04, 0x05, 0xf7
    };

    buffer.write(message, 7);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExStart, packet.header);
    EXPECT_EQ(0xf0, packet.byte1);
    EXPECT_EQ(0x01, packet.byte2);
    EXPECT_EQ(0x02, packet.byte3);
    EXPECT_EQ(4, buffer.getLength());

    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExContinue, packet.header);
    EXPECT_EQ(0x03, packet.byte1);
    EXPECT_EQ(0x04, packet.byte2);
    EXPECT_EQ(0x05, packet.byte3);
    EXPECT_EQ(1, buffer.getLength());

    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExEnds1Byte, packet.header);
    EXPECT_EQ(0xf7, packet.byte1);
    EXPECT_EQ(0x00, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

TEST(MidiUsbPacketInterfaceTx, SysExMultiplePacketsEndingWith2Bytes)
{
    midiEventPacket_t packet;
    Buffer buffer;

    const byte message[8] = {
        0xf0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xf7
    };

    buffer.write(message, 8);
    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExStart, packet.header);
    EXPECT_EQ(0xf0, packet.byte1);
    EXPECT_EQ(0x01, packet.byte2);
    EXPECT_EQ(0x02, packet.byte3);
    EXPECT_EQ(5, buffer.getLength());

    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExContinue, packet.header);
    EXPECT_EQ(0x03, packet.byte1);
    EXPECT_EQ(0x04, packet.byte2);
    EXPECT_EQ(0x05, packet.byte3);
    EXPECT_EQ(2, buffer.getLength());

    EXPECT_TRUE(midi::composeTxPacket(buffer, packet));
    EXPECT_EQ(CIN::sysExEnds2Bytes, packet.header);
    EXPECT_EQ(0x06, packet.byte1);
    EXPECT_EQ(0xf7, packet.byte2);
    EXPECT_EQ(0x00, packet.byte3);
    EXPECT_EQ(0, buffer.getLength());
}

// =============================================================================

TEST(MidiUsbPacketInterfaceRx, InvalidPacketIsNotSerialised)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::reserved;
    packet.byte1 = 0x01;
    packet.byte2 = 0x02;
    packet.byte3 = 0x03;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_TRUE(buffer.isEmpty());
}

TEST(MidiUsbPacketInterfaceRx, CableEventsAreNotSerialised)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::cableEvent;
    packet.byte1 = 0x01;
    packet.byte2 = 0x02;
    packet.byte3 = 0x03;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_TRUE(buffer.isEmpty());
}

// Channel Voice Messages --

TEST(MidiUsbPacketInterfaceRx, NoteOff)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::noteOff;
    packet.byte1 = 0x80;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0x80, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, NoteOn)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::noteOn;
    packet.byte1 = 0x91;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0x91, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, PolyPressure)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::polyPressure;
    packet.byte1 = 0xA2;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xA2, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, ControlChange)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::controlChange;
    packet.byte1 = 0xB3;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xB3, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, ProgramChange)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::programChange;
    packet.byte1 = 0xC4;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(0xC4, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, ChannelPressure)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::channelPressure;
    packet.byte1 = 0xD5;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(0xD5, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, PitchBend)
{
    Buffer buffer;
    midiEventPacket_t packet;
    packet.header = CIN::pitchBend;
    packet.byte1 = 0xE6;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xE6, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
}

// System Real Time Messages --

TEST(MidiUsbPacketInterfaceRx, Clock)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::Clock;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::Clock, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, Start)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::Start;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::Start, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, Stop)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::Stop;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::Stop, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, Continue)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::Continue;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::Continue, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, ActiveSensing)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::ActiveSensing;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::ActiveSensing, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SystemReset)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::singleByte;
    packet.byte1 = midi::SystemReset;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::SystemReset, buffer.read());
}

// System Common Messages --

TEST(MidiUsbPacketInterfaceRx, TuneRequest)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::systemCommon1Byte;
    packet.byte1 = midi::TuneRequest;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(midi::TuneRequest, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SongSelect)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::systemCommon2Bytes;
    packet.byte1 = midi::SongSelect;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(midi::SongSelect, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SongPosition)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::systemCommon2Bytes;
    packet.byte1 = midi::SongPosition;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(midi::SongPosition, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, TimeCodeQuarterFrame)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::systemCommon2Bytes;
    packet.byte1 = midi::TimeCodeQuarterFrame;
    packet.byte2 = 0x12;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(midi::TimeCodeQuarterFrame, buffer.read());
    EXPECT_EQ(0x12, buffer.read());
}

// System Exclusive --

TEST(MidiUsbPacketInterfaceRx, SysExSinglePacket2Bytes)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::sysExEnds2Bytes;
    packet.byte1 = 0xf0;
    packet.byte2 = 0xf7;
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(0xf0, buffer.read());
    EXPECT_EQ(0xf7, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SysExSinglePacket3Bytes)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::sysExEnds3Bytes;
    packet.byte1 = 0xf0;
    packet.byte2 = 0x42;
    packet.byte3 = 0xf7;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xf0, buffer.read());
    EXPECT_EQ(0x42, buffer.read());
    EXPECT_EQ(0xf7, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SysExTwoPackets)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::sysExStart;
    packet.byte1 = 0xf0;
    packet.byte2 = 0x01;
    packet.byte3 = 0x02;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xf0, buffer.read());
    EXPECT_EQ(0x01, buffer.read());
    EXPECT_EQ(0x02, buffer.read());

    packet.header = CIN::sysExEnds3Bytes;
    packet.byte1 = 0x03;
    packet.byte2 = 0x04;
    packet.byte3 = 0xf7;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0x03, buffer.read());
    EXPECT_EQ(0x04, buffer.read());
    EXPECT_EQ(0xf7, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SysExMultiplePacketsEndingWith1Byte)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::sysExStart;
    packet.byte1 = 0xf0;
    packet.byte2 = 0x01;
    packet.byte3 = 0x02;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xf0, buffer.read());
    EXPECT_EQ(0x01, buffer.read());
    EXPECT_EQ(0x02, buffer.read());

    packet.header = CIN::sysExContinue;
    packet.byte1 = 0x03;
    packet.byte2 = 0x04;
    packet.byte3 = 0x05;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0x03, buffer.read());
    EXPECT_EQ(0x04, buffer.read());
    EXPECT_EQ(0x05, buffer.read());

    packet.header = CIN::sysExEnds1Byte;
    packet.byte1 = 0xf7;
    packet.byte2 = 0x12; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(1, buffer.getLength());
    EXPECT_EQ(0xf7, buffer.read());
}

TEST(MidiUsbPacketInterfaceRx, SysExMultiplePacketsEndingWith2Bytes)
{
    midiEventPacket_t packet;
    Buffer buffer;

    packet.header = CIN::sysExStart;
    packet.byte1 = 0xf0;
    packet.byte2 = 0x01;
    packet.byte3 = 0x02;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0xf0, buffer.read());
    EXPECT_EQ(0x01, buffer.read());
    EXPECT_EQ(0x02, buffer.read());

    packet.header = CIN::sysExContinue;
    packet.byte1 = 0x03;
    packet.byte2 = 0x04;
    packet.byte3 = 0x05;
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(3, buffer.getLength());
    EXPECT_EQ(0x03, buffer.read());
    EXPECT_EQ(0x04, buffer.read());
    EXPECT_EQ(0x05, buffer.read());

    packet.header = CIN::sysExEnds2Bytes;
    packet.byte1 = 0x06;
    packet.byte2 = 0xf7; // Should be ignored
    packet.byte3 = 0x42; // Should be ignored
    midi::serialiseRxPacket(packet, buffer);
    EXPECT_EQ(2, buffer.getLength());
    EXPECT_EQ(0x06, buffer.read());
    EXPECT_EQ(0xf7, buffer.read());
}

END_UNNAMED_NAMESPACE
