
#include <SoftwareSerial.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

#include "midi_ValidatorLCD.h"
#include "midi_ValidatorLEDs.h"

// -----------------------------------------------------------------------------

#define NUM_TESTS       1

// -----------------------------------------------------------------------------

SoftwareSerial softSerial(2, 3);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial,        midiHW);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial,    midiSW);
// \todo Create instance for USB if available


// -----------------------------------------------------------------------------
/*
BEGIN_MIDI_NAMESPACE

template<class SerialClass>
class TestFixture
{
public:
    typedef const char* String;
    typedef bool (*Functor) (MidiInterface<SerialClass>&);
    
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

template<class SerialClass>
class MidiTester
{
public:
    explicit MidiTester(MidiInterface<SerialClass>& inInstance)
        : mInstance(inInstance)
    {
    }
    
    virtual ~MidiTester()
    {
    }
    
public:
    inline bool performTestFixture(TestFixture<SerialClass>* inFixture)
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
    inline void registerTest(TestFixture<SerialClass>* inFixture)
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
    MidiInterface<SerialClass>& mInstance;
    TestFixture<SerialClass>* mTests[NUM_TESTS];
    unsigned mNumActiveTests;
};

// -----------------------------------------------------------------------------

template<class SerialClass>
bool testNoteOn(midi::MidiInterface<SerialClass>& inMidi)
{
    inMidi.sendNoteOn(12, 42);
    return true;
}

// -----------------------------------------------------------------------------

END_MIDI_NAMESPACE

midi::MidiTester<HardwareSerial> testerHW(midiHW);
midi::MidiTester<SoftwareSerial> testerSW(midiSW);
*/
// -----------------------------------------------------------------------------

template<class SerialClass>
void setupMidiInstance(midi::MidiInterface<SerialClass>& inInstance)
{
    inInstance.begin(MIDI_CHANNEL_OMNI);
    inInstance.turnThruOff();
}

/*
template<class SerialClass>
void setupTester(midi::MidiTester<SerialClass>& inTester)
{
    inTester.registerTest(new midi::TestFixture("Note On", midi::testNoteOn));
}

void setupTesters()
{
    setupTester<HardwareSerial>(testerHW);
    setupTester<SoftwareSerial>(testerSW);
}
*/

// -----------------------------------------------------------------------------

void setup()
{
    setupMidiInstance<HardwareSerial>(midiHW);
    setupMidiInstance<SoftwareSerial>(midiSW);
    setupLCD();
    setupLEDs();
    //setupTesters();
}

// -----------------------------------------------------------------------------

void loop()
{
    //testerHW.runTests();
    //testerSW.runTests();
}
