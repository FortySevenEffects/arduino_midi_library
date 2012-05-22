#include <MIDI.h>

const int ledPin = 11;   // Teensy has LED on 11, Teensy++ on 6

void setup() {
  MIDI.begin();
  pinMode(ledPin,OUTPUT);
}

void loop() {
  if (MIDI.read()) {
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(50);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    //delay(100);                  // wait for a second
  }
}



