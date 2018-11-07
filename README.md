# Arduino MIDI Library

[![Build Status](https://travis-ci.org/FortySevenEffects/arduino_midi_library.svg?branch=master)](https://travis-ci.org/FortySevenEffects/arduino_midi_library)
[![Coveralls](https://img.shields.io/coveralls/FortySevenEffects/arduino_midi_library.svg?maxAge=3600)](https://coveralls.io/github/FortySevenEffects/arduino_midi_library)
[![GitHub release](https://img.shields.io/github/release/FortySevenEffects/arduino_midi_library.svg?maxAge=3600)](https://github.com/FortySevenEffects/arduino_midi_library/releases/latest)
[![License](https://img.shields.io/github/license/FortySevenEffects/arduino_midi_library.svg?maxAge=3600)](LICENSE)

This library adds MIDI I/O communications to an Arduino board.

### Features

-   **New** : USB MIDI Device support with [`MIDIUSB`](https://github.com/arduino-libraries/MIDIUSB).
-   Compatible with all Arduino boards (and clones with an AVR processor).
-   Simple and fast way to send and receive every kind of MIDI message (including all System messages, SysEx, Clock, etc..).
-   OMNI input reading (read all channels).
-   Software Thru, with message filtering.
-   [Callbacks](https://github.com/FortySevenEffects/arduino_midi_library/wiki/Using-Callbacks) to handle input messages more easily.
-   Last received message is saved until a new one arrives.
-   Configurable: [overridable template-based settings](https://github.com/FortySevenEffects/arduino_midi_library/wiki/Using-custom-Settings).
-   Create more than one MIDI interface for mergers/splitters applications.
-   Use any serial port, hardware or software.

### Getting Started

1. Use the Arduino Library Manager to install the library.
   ![Type "MIDI" in the Arduino IDE Library Manager](res/library-manager.jpg)

2. Start coding:

```c++
#include <MIDI.h>

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
}

void loop()
{
    // Send note 42 with velocity 127 on channel 1
    MIDI.sendNoteOn(42, 127, 1);

    // Read incoming messages
    MIDI.read();
}
```

3. Read the [documentation](#documentation) or watch the awesome video tutorials from [Notes & Volts](https://www.youtube.com/playlist?list=PL4_gPbvyebyH2xfPXePHtx8gK5zPBrVkg).

## Documentation

-   [Doxygen Extended Documentation](https://fortyseveneffects.github.io/arduino_midi_library/).
-   [GitHub wiki](https://github.com/FortySevenEffects/arduino_midi_library/wiki).

## Contact

To report a bug, contribute, discuss on usage, or simply request support, please [create an issue here](https://github.com/FortySevenEffects/arduino_midi_library/issues/new).

You can also contact me on Twitter: [@fortysevenfx](https://twitter.com/fortysevenfx).

## License

MIT © 2009 - present [Francois Best](https://francoisbest.com)
