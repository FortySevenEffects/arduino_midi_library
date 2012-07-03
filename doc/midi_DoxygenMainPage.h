/*!
 \mainpage Arduino MIDI Library
 
 \tableofcontents
 
 
 \page howtodoc How to use this documentation

  You can browse the classes 
 
 \page howtolib How to use the library
 
 \section howtolib_section_callbacks Using callbacks
 
 Callbacks can simplify the reception of input messages.
 
 
 
 \page 
 
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

