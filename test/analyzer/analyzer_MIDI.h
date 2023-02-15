#pragma once

#include "analyzer.h"
#include <test/mocks/test-mocks_SerialMock.h>
#include <src/MIDI.h>

BEGIN_ANALYZER_NAMESPACE

using SerialMock    = test_mocks::SerialMock<32>;
using Transport     = midi::SerialMIDI<SerialMock>;
using MidiInterface = midi::MidiInterface<Transport>;

struct MIDIAnalyzer {
public:
    MIDIAnalyzer();

public:
    void setup();
    void process(uint8_t inByte);

public:
    SerialMock mSerialBuffer;
    Transport mTransport;
    MidiInterface mMIDI;
};

END_ANALYZER_NAMESPACE
