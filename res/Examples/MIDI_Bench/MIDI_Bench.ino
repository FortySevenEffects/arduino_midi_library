#include <SoftwareSerial.h>
#define MIDI_AUTO_INSTANCIATE 0
#include <MIDI.h>

// This program demonstrates how to use two serial ports at a time,
// the hardware serial being used for sending messages to the computer,
// while MIDI is handled on a software serial port.
// This program measures the time spent to receive and parse a message.

// -----------------------------------------------------------------------------

SoftwareSerial midiSerial(2,3);
MIDI_CREATE_INSTANCE(SoftwareSerial, midiSerial, softMidi);

unsigned long gTime_start = 0;
unsigned long gTime_stop  = 0;
unsigned gCounter = 0;
unsigned long gTime_sum   = 0;
unsigned long gTime_min   = -1;
unsigned long gTime_max   = 0;

// -----------------------------------------------------------------------------

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
    gTime_stop = micros();
  
    const unsigned long diff = gTime_stop - gTime_start;
    gTime_sum += diff;
  
    if (diff > gTime_max) gTime_max = diff;
    if (diff < gTime_min) gTime_min = diff;
  
    gCounter++;
  
    if (gCounter >= 100)
    {
        const unsigned long average = gTime_sum / (float)gCounter;
        
        Serial.println("Time to receive NoteOn: ");
        
        Serial.print("Average: ");
        Serial.print(average);
        Serial.println(" microsecs");
        
        Serial.print("Min:     ");
        Serial.print(gTime_min);
        Serial.println(" microsecs");
        
        Serial.print("Max:     ");
        Serial.print(gTime_max);
        Serial.println(" microsecs");
        
        gCounter = 0;
        gTime_sum = 0;
        gTime_max = 0;
        gTime_min = -1;
        
        softMidi.turnThruOff();
    }
}

// -----------------------------------------------------------------------------

void setup()
{
    softMidi.begin();
  
    Serial.begin(38400);
    Serial.print("Arduino Ready");
  
    softMidi.sendNoteOn(69,127,1);
}


void loop()
{
    gTime_start = micros();
    softMidi.read();
}

