# -*- coding: utf-8 -*-


# ------------------------------------------------------------------------------

class Tester:
    def __init__(self, interface):
        self.interface  = interface
        self.sent       = None
        self.expected   = None
        self.received   = None

    def handleMidiInput(self, data):
        print('Recived data:', data)
        self.received = data

    def checkThru(self, message):
        self.interface.send(message)
        self.sent     = message
        self.expected = message
        self.received = None
        while not self.received:
            pass
        return self.expected == self.received
