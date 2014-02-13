#include <Arduino.h>
#include "midi_ValidatorSerialDebug.h"

void setupSerialDebug()
{
	Serial.begin(9600);
	while (!Serial)
	{
        ; // wait for serial port to connect. Needed for Leonardo only
    }
}

