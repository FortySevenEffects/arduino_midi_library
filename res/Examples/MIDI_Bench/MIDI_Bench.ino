#include <MIDI.h>

// This program will measure the time needed to receive, parse and process a
// NoteOn message.
// For it to work, please connect RX and TX on the MIDI port:
// Due, Leonardo and other USB-native Arduinos:     Serial1
// All other Arduinos:                              Connect pins 2 and 3.
// The program will then wait for 100 loops and print the results.

#if defined(ARDUINO_SAM_DUE) || defined(USBCON)
    // Print through USB and bench with Hardware serial
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midiBench);
#else
    #include <SoftwareSerial.h>
    SoftwareSerial midiSerial(2,3);
    MIDI_CREATE_INSTANCE(SoftwareSerial, midiSerial, midiBench);
#endif

// -----------------------------------------------------------------------------

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

    if (gCounter++ >= 1000)
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

        midiBench.turnThruOff();
    }
}

// -----------------------------------------------------------------------------

void setup()
{
    midiBench.setHandleNoteOn(handleNoteOn);
    midiBench.begin();

    while(!Serial);
    Serial.begin(115200);
    Serial.println("Arduino Ready");

    midiBench.sendNoteOn(69,127,1);
}

void loop()
{
    gTime_start = micros();
    midiBench.read();
}
