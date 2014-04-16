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
        self.input              = rtmidi.MidiIn()
        self.output             = rtmidi.MidiOut()
        self.listenerCallback   = listenerCallback
        self.ports              = self.getAvailablePorts()
        self.port               = self.connect(self.choosePorts())

    # --------------------------------------------------------------------------

    def handleMidiInput(self, message, timestamp):
        midiData = message[0]
        if self.listenerCallback:
            self.listenerCallback(midiData)

    def send(self, message):
        print('Sending', message)
        self.output.send_message(message)

    # --------------------------------------------------------------------------

    def getAvailablePorts(self):
        return {
            'input' : self.input.get_ports(),
            'output': self.output.get_ports(),
        }

    def choosePorts(self):
        return {
            'input' : self.choosePort(self.ports['input'],  'input'),
            'output': self.choosePort(self.ports['output'], 'output')
        }

    def choosePort(self, ports, direction):
        if not ports:
            print('No MIDI ports available, bailing out.')
            return None

        if len(ports) == 1:
            return {
                'id':   0,
                'name': ports[0]
            }

        else:
            # Give a choice
            print('Multiple %s ports available, please make a choice:' % direction)
            choices = dict()
            for port, i in zip(ports, range(0, len(ports))):
                choices[i] = port
                print('  [%d]' % i, port)
            choiceIndex = int(input('-> '))
            return {
                'id': choiceIndex,
                'name': choices[choiceIndex]
            }

    # --------------------------------------------------------------------------

    def connect(self, ports):
        if not ports:
            return None

        print('Connecting input to %s'  % ports['input']['name'])
        print('Connecting output to %s' % ports['output']['name'])

        self.input.set_callback(self.handleMidiInput)
        self.input.open_port(ports['input']['id'])
        self.output.open_port(ports['output']['id'])
        return ports
