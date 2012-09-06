#include <MIDI.h>

unsigned long gTime_start = 0;
unsigned long gTime_stop  = 0;
unsigned gCounter = 0;
unsigned long gTime_sum   = 0;
unsigned long gTime_min   = -1;
unsigned long gTime_max   = 0;


void handleNoteOn(byte inChannel,byte inNote,byte inVelocity)
{
  
  gTime_stop = micros();
  
  const unsigned long diff = gTime_stop - gTime_start;
  gTime_sum += diff;
  
  if (diff > gTime_max) gTime_max = diff;
  if (diff < gTime_min) gTime_min = diff;
  
  gCounter++;
  
  if (gCounter >= 100) {
    
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
    
    MIDI.turnThruOff();
    
  }
  
}


void setup()
{
  
  MIDI.begin();
  
  Serial.begin(38400);
  Serial.print("MCU Ready");
  
  MIDI.sendNoteOn(69,127,1);
  
}


void loop()
{
  
  gTime_start = micros();
  
  MIDI.read();
  
}

