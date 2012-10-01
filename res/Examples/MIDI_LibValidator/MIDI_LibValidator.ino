
#include <SoftwareSerial.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

// -----------------------------------------------------------------------------

#define LCD_D4          8
#define LCD_D5          9
#define LCD_D6          10
#define LCD_D7          11
#define LCD_RS          12
#define LCD_EN          13

#define LED_PASS        4
#define LED_FAIL        5

// -----------------------------------------------------------------------------

SoftwareSerial softSerial(2, 3);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial,        midiHW);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
// \todo Create instance for USB if available

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// -----------------------------------------------------------------------------

template<class SerialClass>
void setupMidiInstance(midi::MidiInterface<SerialClass>& inInstance)
{
    inInstance.begin(MIDI_CHANNEL_OMNI);
    inInstance.turnThruOff();
}

void setupLCD()
{
    lcd.begin(16,2);
}

void setupLEDs()
{
    pinMode(LED_PASS, OUTPUT);
    pinMode(LED_FAIL, OUTPUT);
}

// -----------------------------------------------------------------------------

void setup()
{
    setupMidiInstance<HardwareSerial>(midiHW);
    setupMidiInstance<SoftwareSerial>(midiSW);
    setupLCD();
    setupLEDs();
}

// -----------------------------------------------------------------------------

void loop()
{
    
}
