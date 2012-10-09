
#pragma once

#include <SoftwareSerial.h>
#include <MIDI.h>

extern midi::MidiInterface<HardwareSerial> midiHW;
extern midi::MidiInterface<SoftwareSerial> midiSW;

void setupMidi();
