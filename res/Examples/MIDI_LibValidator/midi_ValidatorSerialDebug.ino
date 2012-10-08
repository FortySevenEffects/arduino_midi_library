#include "midi_ValidatorSerialDebug.h"

void setupSerialDebug()
{
	Serial.begin(38400);
}

void print(const char* inString)
{
	Serial.println(inString);
}

