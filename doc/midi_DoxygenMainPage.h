/*!
 \mainpage Arduino MIDI Library

 See the documentation of the main class, MidiInterface, or browse the modules
 and examples using the sidebar on the left.
 \n
 The latest development version is available on GitHub:
 https://github.com/FortySevenEffects/arduino_midi_library/tree/dev
 */

// -----------------------------------------------------------------------------
// Examples

/*!
 \example MIDI_Basic_IO.ino
 This example shows how to perform simple input and output MIDI. \n
 \n
 When any message arrives to the Arduino, the LED is turned on,
 then we send a Note On message, wait for a second, then send
 the Note Off and turn off the LED.
 \n
 \n
 <em>
 Note that instead of sending a Note Off, we could have sent a
 Note On with velocity 0 to shorten the message. This is called Running
 Status.
 </em>
 \n
 */

/*!
 \example MIDI_Callbacks.ino
 This example shows how to use callbacks for easier MIDI input handling. \n
 */

/*!
 \example MIDI_Bench.ino
 \example MIDI_DualMerger.ino
 \example MIDI_Input.ino
 \example MIDI_SimpleSynth.ino
 */

// -----------------------------------------------------------------------------

/*! \defgroup output   MIDI Output
 */
/*! \defgroup input    MIDI Input
 */
/*! \defgroup callbacks Callbacks
 \ingroup input
 */
/*! \defgroup thru     MIDI Thru
 */

