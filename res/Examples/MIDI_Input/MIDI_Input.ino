#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// -----------------------------------------------------------------------------

// This example shows the old way of checking for input messages.
// It's simpler to use the callbacks now, check out the dedicated example.

#define LED 13                   // LED pin on Arduino Uno

// -----------------------------------------------------------------------------

void BlinkLed(byte num)         // Basic blink function
{
    for (byte i=0;i<num;i++)
    {
        digitalWrite(LED,HIGH);
        delay(50);
        digitalWrite(LED,LOW);
        delay(50);
    }
}

// -----------------------------------------------------------------------------

void setup()
{
    pinMode(LED, OUTPUT);
    MIDI.begin();           // Launch MIDI, by default listening to channel 1.
}

void loop()
{
    if (MIDI.read())                // Is there a MIDI message incoming ?
    {
        switch(MIDI.getType())      // Get the type of the message we caught
        {
            case midi::ProgramChange:       // If it is a Program Change,
                BlinkLed(MIDI.getData1());  // blink the LED a number of times
                                            // correponding to the program number
                                            // (0 to 127, it can last a while..)
                break;
            // See the online reference for other message types
            default:
                break;
        }
    }
}
