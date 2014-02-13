#pragma once

#define LED_PASS        4
#define LED_FAIL        5

// -----------------------------------------------------------------------------

void setupLEDs();

// -----------------------------------------------------------------------------

inline void blinkLed(byte inLedNum)
{
    digitalWrite(inLedNum, HIGH);
    delay(100);
    digitalWrite(inLedNum, LOW);
}

inline void blinkPass()
{
    blinkLed(LED_PASS);
}

inline void blinkFail()
{
    blinkLed(LED_FAIL);
}

inline void setLedsFinal(bool inSuccess)
{
    if (inSuccess)
    {
        digitalWrite(LED_PASS, HIGH);
        digitalWrite(LED_FAIL, LOW);
    }
    else
    {
        digitalWrite(LED_FAIL, HIGH);
        digitalWrite(LED_PASS, LOW);
    }
}

// -----------------------------------------------------------------------------

