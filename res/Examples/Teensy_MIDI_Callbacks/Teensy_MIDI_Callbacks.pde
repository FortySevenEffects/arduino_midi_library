#include <MIDI.h>

const int ledPin = 11;   // Teensy has LED on 11, Teensy++ on 6

// NoteOn Callback (will be called upon reception of a NoteOn message 
void TurnLedOn(byte inChannel,byte inNote,byte inVelocity) {
  if (inVelocity == 0) {
    // NoteOn + null velo = note off.
    digitalWrite(ledPin,LOW);
  }
  else digitalWrite(ledPin,HIGH);
}

// NoteOff Callback (will be called upon reception of a NoteOff message
void TurnLedOff(byte inChannel,byte inNote,byte inVelocity) {
  digitalWrite(ledPin,LOW);
}

void setup() {
  pinMode(ledPin,OUTPUT);
  MIDI.begin();
  MIDI.setHandleNoteOn(TurnLedOn);    // Connect callback for NoteOn
  MIDI.setHandleNoteOff(TurnLedOff);  // Connect callback for NoteOff
}

void loop() {
  MIDI.read();  // Keep reading MIDI input
}



