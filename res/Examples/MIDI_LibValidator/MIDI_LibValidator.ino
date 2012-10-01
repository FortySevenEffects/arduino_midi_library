
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

#define NUM_TESTS       1

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

SoftwareSerial softSerial(2, 3);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial,        midiHW);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
// \todo Create instance for USB if available

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// -----------------------------------------------------------------------------

BEGIN_MIDI_NAMESPACE

class TestFixture
{
public:
    typedef const char* String;
    typedef bool (*Functor) (void);
    
public:
    TestFixture(String inTestName, 
                Functor inFunctor)
        : mTestName(inTestName)
        , mFunctor(inFunctor)
    {
    }
    
public:
    inline bool operator() (void)
    {
        return mFunctor();
    }
    
    inline String getName() const
    {
        return mTestName;
    }
    
private:
    String mTestName;
    Functor mFunctor;
};

// -----------------------------------------------------------------------------

template<class SerialPort>
class MidiTester
{
public:
    explicit MidiTester(MidiInterface<SerialPort>& inInstance)
        : mInstance(inInstance)
    {
    }
    
    virtual ~MidiTester()
    {
    }
    
public:
    inline bool performTestFixture(TestFixture* inFixture)
    {
        if (inFixture != 0)
        {
            return (*inFixture)();
        }
        return false;
    }
    
    inline bool expect(bool inCondition) const
    {
        if (inCondition == false)
            blinkFail();
        else
            blinkPass();
        return inCondition;
    }
    
public:
    inline void registerTest(TestFixture* inFixture)
    {
        mTests[mNumActiveTests++] = inFixture;
    }
    
    bool runTests()
    {
        for (unsigned i = 0; i < mNumActiveTests; ++i)
        {
            if (mTests[i] != 0)
            {
                if (expect(performTestFixture(mTests[i]) == false))
                {
                    // \todo Display message
                    // \todo Store error number and continue.
                    return false;
                }
            }
            setProgressBar(i, mNumActiveTests);
        }
        return true;
    }
    
private:
    MidiInterface<SerialPort>& mInstance;
    TestFixture* mTests[NUM_TESTS];
    unsigned mNumActiveTests;
};

// -----------------------------------------------------------------------------

bool testNoteOn()
{
    return true;
}

// -----------------------------------------------------------------------------

END_MIDI_NAMESPACE

midi::MidiTester<HardwareSerial> testerHW(midiHW);
midi::MidiTester<SoftwareSerial> testerSW(midiSW);

// -----------------------------------------------------------------------------

template<class SerialClass>
void setupMidiInstance(midi::MidiInterface<SerialClass>& inInstance)
{
    inInstance.begin(MIDI_CHANNEL_OMNI);
    inInstance.turnThruOff();
}

void setupLCD()
{
    for (byte i = 0; i < 6; ++i)
        lcd.createChar(i, progressChar[i]);
    
    lcd.begin(16,2);
}

void setupLEDs()
{
    pinMode(LED_PASS, OUTPUT);
    pinMode(LED_FAIL, OUTPUT);
}

template<class SerialClass>
void setupTester(MidiTester<SerialClass>& inTester)
{
    inTester.registerTest(new midi::TestFixture("Note On", midi::testNoteOn));
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

void setup()
{
    setupMidiInstance<HardwareSerial>(midiHW);
    setupMidiInstance<SoftwareSerial>(midiSW);
    setupLCD();
    setupLEDs();
    setupTesters();
}

// -----------------------------------------------------------------------------

void loop()
{
    testerHW.runTests();
    testerSW.runTests();
}
