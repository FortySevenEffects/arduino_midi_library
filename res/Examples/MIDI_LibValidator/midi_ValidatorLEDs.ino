#include <Arduino.h>
#include "midi_ValidatorLEDs.h"

// -----------------------------------------------------------------------------

void setupLEDs()
{
    pinMode(LED_PASS, OUTPUT);
    pinMode(LED_FAIL, OUTPUT);
}
