#pragma once
#include <LiquidCrystal.h>

// -----------------------------------------------------------------------------

extern LiquidCrystal lcd;

// -----------------------------------------------------------------------------

void setupLCD();

// -----------------------------------------------------------------------------

void setProgressBar(unsigned inProgress, unsigned inTotal);
