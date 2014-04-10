# -*- coding: utf-8 -*-

import rtmidi
import random

# ------------------------------------------------------------------------------

class Midi:
    InvalidType           = 0x00    # For notifying errors
    NoteOff               = 0x80    # Note Off
    NoteOn                = 0x90    # Note On
    AfterTouchPoly        = 0xA0    # Polyphonic AfterTouch
    ControlChange         = 0xB0    # Control Change / Channel Mode
    ProgramChange         = 0xC0    # Program Change
    AfterTouchChannel     = 0xD0    # Channel (monophonic) AfterTouch
    PitchBend             = 0xE0    # Pitch Bend
    SystemExclusive       = 0xF0    # System Exclusive
    TimeCodeQuarterFrame  = 0xF1    # System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2    # System Common - Song Position Pointer
    SongSelect            = 0xF3    # System Common - Song Select
    TuneRequest           = 0xF6    # System Common - Tune Request
    Clock                 = 0xF8    # System Real Time - Timing Clock
    Start                 = 0xFA    # System Real Time - Start
    Continue              = 0xFB    # System Real Time - Continue
    Stop                  = 0xFC    # System Real Time - Stop
    ActiveSensing         = 0xFE    # System Real Time - Active Sensing
    SystemReset           = 0xFF    # System Real Time - System Reset

    @staticmethod
    def getChannel(statusByte):
        return statusByte & 0x0f;

    @staticmethod
    def getType(statusByte):
        if statusByte >= 0xf0:
            # System messages
            return statusByte
        else:
            # Channel messages
            return statusByte & 0xf0;


# ------------------------------------------------------------------------------

class MidiInterface:
    def __init__(self, listenerCallback = None):
        self.interface          = rtmidi.MidiIn()
        self.listenerCallback   = listenerCallback
        self.ports              = self.getAvailablePorts()
        self.port               = self.connect(self.choosePort())

    # --------------------------------------------------------------------------

    def read(self):
        pass

    def handleMidiInput(self, message, timestamp):
        midiData = message[0]
        print(midiData)
        if self.listenerCallback:
            self.listenerCallback(midiData)

    # --------------------------------------------------------------------------

    def getAvailablePorts(self):
        return self.interface.get_ports()

    def choosePort(self):
        if not self.ports:
            print('No MIDI ports available, bailing out.')
            return None

        if len(self.ports) == 1:
            return (0, self.ports[0])

        else:
            # Give a choice
            print('Multiple ports available, please make a choice:')
            choices = []
            for port, i in zip(self.ports, range(0, len(self.ports))):
                choices.append((i, port))
                print('  [%d]' % i, port)
            choiceIndex = int(input('-> '))
            return choices[choiceIndex]

    # --------------------------------------------------------------------------

    def connect(self, port):
        if not port:
            return None
        print(port)
        print('Connecting to %s' % port[1])
        self.interface.set_callback(self.handleMidiInput)
        self.interface.open_port(port[0])
        return port


midi = MidiInterface()

while True:
    midi.read()
