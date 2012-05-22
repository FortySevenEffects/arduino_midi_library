#include <MIDI.h>

const int ledPin = 11;   // Teensy has LED on 11, Teensy++ on 6

void Callback(word data) {
  digitalWrite(ledPin,HIGH);
  delay(100);
  digitalWrite(ledPin,LOW);
  delay(100);
}

void setup() {
  pinMode(ledPin,OUTPUT);
  MIDI.begin();  // Start MIDI on DIN plugs
  MIDI.setHandleSongPosition(Callback);
}

void loop() {
  MIDI.read();
}



