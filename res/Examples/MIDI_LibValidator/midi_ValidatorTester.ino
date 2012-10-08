
#include "midi_ValidatorTester.h"
#include "midi_ValidatorInstances.h"
/*
// -----------------------------------------------------------------------------

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
bool testNoteOn(MIDI_CLASS(SerialClass)& inMidi)
{
    inMidi.sendNoteOn(12, 42);
    return true;
}

// -----------------------------------------------------------------------------

END_MIDI_NAMESPACE

midi::MidiTester<HardwareSerial> testerHW(midiHW);
midi::MidiTester<SoftwareSerial> testerSW(midiSW);

// -----------------------------------------------------------------------------

template<class SerialClass>
void setupTester(MIDI_CLASS(SerialClass)& inTester)
{
    inTester.registerTest(new midi::TestFixture("Note On", midi::testNoteOn));
}

// -----------------------------------------------------------------------------
*/
void setupTesters()
{
    //setupTester<HardwareSerial>(testerHW);
    //setupTester<SoftwareSerial>(testerSW);
}

void launchTests()
{
    //testerHW.runTests();
    //testerSW.runTests();
}
