#Arduino MIDI Library v4.2

This library enables MIDI I/O communications on the Arduino serial ports.
The purpose of this library is not to make a big synthesizer out of an Arduino board, the application remains yours. However, it will help you interfacing it with other MIDI devices.

Download the latest version [here](https://github.com/FortySevenEffects/arduino_midi_library/releases/latest).

### Features
* Compatible with all Arduino boards (and clones with an AVR processor).
* Simple and fast way to send and receive every kind of MIDI message (including all System messages, SysEx, Clock, etc..).
* OMNI input reading (read all channels).
* Software Thru, with message filtering.
* [Callbacks](http://playground.arduino.cc/Main/MIDILibraryCallbacks) to handle input messages more easily.
* Last received message is saved until a new one arrives.
* Configurable: [overridable template-based settings](http://arduinomidilib.fortyseveneffects.com/a00013.html#details).
* Create more than one MIDI port for mergers/splitters applications.
* Use any serial port, hardware or software.


#### Changelog
* 11/06/2014 : Version 4.2 released. Bug fix for SysEx, overridable template settings.
* 16/04/2014 : Version 4.1 released. Bug fixes regarding running status.
* 13/02/2014 : Version 4.0 released. Moved to GitHub, added multiple instances & software serial support, and a few bug fixes.
* 29/01/2012 : Version 3.2 released. Release notes are [here](http://sourceforge.net/news/?group_id=265194).
* 06/05/2011 : Version 3.1 released. Added [callback](http://playground.arduino.cc/Main/MIDILibraryCallbacks) support.
* 06/03/2011 : Version 3.0 released. Project is now hosted on [SourceForge](http://sourceforge.net/projects/arduinomidilib).
* 14/12/2009 : Version 2.5 released.
* 28/07/2009 : Version 2.0 released.
* 28/03/2009 : Simplified version of MIDI.begin, Fast mode is now on by default.
* 08/03/2009 : Thru method operational. Added some features to enable thru.



**__Warning: this library requires Arduino 1.0 or more recent versions.__**


### What do I need to do?

* Download the library ([link](https://github.com/FortySevenEffects/arduino_midi_library/releases/latest)).
* Follow the installation instructions - http://arduino.cc/en/Guide/Libraries.
* Include the library in your sketch using the menu in the IDE, or type `#include <MIDI.h>`
* Create the MIDI instance using `MIDI_CREATE_DEFAULT_INSTANCE();` or take a look at the documentation for custom serial port, settings etc..

You are now ready to use the library. Look at the documentation to learn how to use it, or checkout the examples installed with the Library.
You can also watch the awesome video tutorials from [Notes & Volts](https://www.youtube.com/playlist?list=PL4_gPbvyebyH2xfPXePHtx8gK5zPBrVkg).


##Documentation

See the extended documentation [here](http://arduinomidilib.fortyseveneffects.com) ([Mirror](http://fortyseveneffects.github.io/arduino_midi_library/)).

### Using MIDI.begin

In the `setup()` function of the Arduino, you must call the `MIDI.begin()` method. If you don't give any argument to this method, the input channel for MIDI in will be set to 1 (channels are going from 1 to 16, plus `MIDI_CHANNEL_OMNI to listen to all channels at the same time).

This method will:
* Start the serial port at the MIDI baudrate (31250).
* Set the input channel at the argument given (if any, else 1).
* Enable Soft Thru, without filtering (everything at the input is sent back).



### MIDI Thru

The MIDI Thru allows you to redirect your incoming messages to the MIDI output. It replaces the need of a MIDI Thru connector, as it copies every valid incoming message from the input. For good performance, you might want to call read() in a fast loop, for low latency.

Incoming unread bytes/messages are kept in the Arduino serial buffer, in order not to flood it, check regularily with MIDI.read. See the documentation for Thru explanations.

Thru is enabled by default, you can turn it off using appropriate methods.


### Hardware

Take a look at [the MIDI.org schematic](http://www.midi.org/techspecs/electrispec.php).


## Contact
if you have any comment or support request to make, feel free to contact me: francois.best@fortyseveneffects.com

You can also get informations about bug fixes and updates on my twitter account: [@fortysevenfx](http://twitter.com/fortysevenfx).

## License

MIT © 2015 [Francois Best](http://fortyseveneffects.com)
