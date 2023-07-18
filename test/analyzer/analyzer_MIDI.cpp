#include "analyzer_MIDI.h"
#include <iostream>

BEGIN_ANALYZER_NAMESPACE

void handleNoteOn(byte inChannel, byte inPitch, byte inVelocity)
{
    std::cout << "NoteOn            Ch " << int(inChannel) << " Pitch " << int(inPitch) << " Vel " << int(inVelocity);
    if (inPitch > 127) {
        std::cout << "--------------- Pitch greater than 127 detected ";
    }
    if (inVelocity > 127) {
        std::cout << "--------------- Velocity greater than 127 detected ";
    }
    std::cout << std::endl;
}
void handleNoteOff(byte inChannel, byte inPitch, byte inVelocity)
{
    std::cout << "NoteOff           Ch " << int(inChannel) << " Pitch " << int(inPitch) << " Vel " << int(inVelocity);
    if (inPitch > 127) {
        std::cout << "--------------- Pitch greater than 127 detected ";
    }
    if (inVelocity > 127) {
        std::cout << "--------------- Velocity greater than 127 detected ";
    }
    std::cout << std::endl;
}
void handleControlChange(byte inChannel, byte inControl, byte inValue)
{
    std::cout << "ControlChange     Ch " << int(inChannel) << " Cntrl " << int(inControl) << " Val " << int(inValue);
    if (inControl > 127) {
        std::cout << "--------------- Control greater than 127 detected ";
    }
    if (inValue > 127) {
        std::cout << "--------------- Value greater than 127 detected ";
    }
    std::cout << std::endl;
}
void handleProgramChange(byte inChannel, byte inProgram)
{
    std::cout << "ProgramChange     Ch " << int(inChannel) << " Progm " << int(inProgram);
    if (inProgram > 127) {
        std::cout << "--------------- Program greater than 127 detected ";
    }
    std::cout << std::endl;
}
void handleChannelPressure(byte inChannel, byte inPressure)
{
    std::cout << "AftertouchChannel Ch " << int(inChannel) << " Press " << int(inPressure);
    if (inPressure > 127) {
        std::cout << "--------------- Pressure greater than 127 detected ";
    }
    std::cout << std::endl;
}

MIDIAnalyzer::MIDIAnalyzer()
    : mSerialBuffer()
    , mTransport(mSerialBuffer)
    , mMIDI((Transport&)mTransport)
{
}

void MIDIAnalyzer::setup()
{
    mMIDI.begin(MIDI_CHANNEL_OMNI);
    mMIDI.turnThruOff();
    mMIDI.setHandleNoteOn(handleNoteOn);
    mMIDI.setHandleNoteOff(handleNoteOff);
    mMIDI.setHandleControlChange(handleControlChange);
    mMIDI.setHandleProgramChange(handleProgramChange);
    mMIDI.setHandleAfterTouchChannel(handleChannelPressure);
}

void MIDIAnalyzer::process(uint8_t inByte)
{
    std::cout << "Processing byte " << std::hex <<int(inByte) << std::dec << std::endl;
    mSerialBuffer.mRxBuffer.write(inByte);
    mMIDI.read();
}

END_ANALYZER_NAMESPACE
