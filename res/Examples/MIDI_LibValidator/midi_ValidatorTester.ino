#include <Arduino.h>
#include "midi_ValidatorTester.h"
#include "midi_ValidatorInstances.h"
#include "midi_ValidatorTests.h"

// -----------------------------------------------------------------------------

template<class SerialClass>
class Tester
{
public:
    typedef bool (*Functor) (MIDI_CLASS(SerialClass)&);

public:
    explicit Tester(MIDI_CLASS(SerialClass)& inInstance)
        : mMidiInstance(inInstance)
        , mProgress(0)
    {
    }

    void setup()
    {
        mProgress = 0;
        mMidiInstance.begin(MIDI_CHANNEL_OMNI);
        mMidiInstance.turnThruOff();
    }

public:
    inline bool performTest(Functor inTestMethod)
    {
        if (inTestMethod != 0)
        {
            const bool result = expect((*inTestMethod)(mMidiInstance));
            setProgressBar(++mProgress, NUM_TESTS);
            return result;
        }
        return false;
    }

    inline bool expect(bool inCondition) const
    {
        Serial.print(testNames[mProgress]);
        Serial.print(": ");
        if (inCondition == false)
        {
            Serial.println("Failed /!\\");
            blinkFail();
        }
        else
        {
            Serial.println("Passed");
            blinkPass();
        }
        return inCondition;
    }

    bool run()
    {
        bool result = true;
        result &= performTest(testNoteOn);
        result &= performTest(testNoteOff);
        result &= performTest(testControlChange);
        result &= performTest(testProgramChange);
        result &= performTest(testAftertouchMono);
        result &= performTest(testAftertouchPoly);
        result &= performTest(testPitchBend);
        result &= performTest(testSysEx);
        result &= performTest(testClock);
        result &= performTest(testStart);
        result &= performTest(testStop);
        result &= performTest(testContinue);
        result &= performTest(testActiveSensing);
        result &= performTest(testTimeCode);
        result &= performTest(testSongSelect);
        result &= performTest(testSongPosition);
        result &= performTest(testTuneRequest);
        result &= performTest(testSystemReset);
        return result;
    }

private:
    MIDI_CLASS(SerialClass)& mMidiInstance;
    unsigned mProgress;
};

// -----------------------------------------------------------------------------

Tester<HardwareSerial> testerHW(midiHW);
Tester<SoftwareSerial> testerSW(midiSW);

// -----------------------------------------------------------------------------

void setupTesters()
{
    testerHW.setup();
    testerSW.setup();
    setupMidi();
}

bool launchTests()
{
    Serial.println("Testing HW:");
    if (testerHW.run() == false)
        return false;
    Serial.println("Testing SW:");
    if (testerSW.run() == false)
        return false;
    return true;
}
