#include <Arduino.h>
#include "midi_ValidatorLCD.h"

#define LCD_D4      8
#define LCD_D5      9
#define LCD_D6      10
#define LCD_D7      11
#define LCD_RS      12
#define LCD_EN      13

// -----------------------------------------------------------------------------

byte progressChar[6][8] =
{
    { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 },
    { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000 },
    { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000 },
    { B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100 },
    { B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110 },
    { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 },
};

// -----------------------------------------------------------------------------

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setupLCD()
{
    for (byte i = 0; i < 6; ++i)
        lcd.createChar(i, progressChar[i]);

    lcd.begin(16,2);
}

// -----------------------------------------------------------------------------

void setProgressBar(unsigned inProgress, unsigned inTotal)
{
    const byte numCols = 16;
    const byte numPix = 5;
    const unsigned progress = (inProgress * numCols* numPix) / inTotal;
    const byte cursorX = progress / numPix;
    const byte charIndex = progress % numPix;

    lcd.setCursor(cursorX, 1);
    lcd.write(charIndex);
}
