#include <MIDI.h>
#include "utility.h"

MIDI_CREATE_DEFAULT_INSTANCE();

/* Listen to RPN & NRPN messages on all channels

The complexity of this example resides in the fact that keeping a state
of all the 16384 * 2 RPN/NRPN values would not fit in memory.
As we're only interested in a few of them, we use a separate state map.

If you'd like to go further, have a look at this thread:
https://github.com/FortySevenEffects/arduino_midi_library/issues/60
*/

template<class State, byte MsbSelectCCNumber, byte LsbSelectCCNumber>
class ParameterNumberParser
{
public:
    ParameterNumberParser(State& inState)
        : mState(inState)
    {
    }

public:
    inline void reset()
    {
        mState.reset();
        mSelected = false;
        mCurrentNumber = 0;
    }

public:
    bool parseControlChange(byte inNumber, byte inValue)
    {
        switch (inNumber)
        {
            case MsbSelectCCNumber:
                mCurrentNumber.mMsb = inValue;
                break;
            case LsbSelectCCNumber:
                if (inValue == 0x7f && mCurrentNumber.mMsb == 0x7f)
                {
                    // End of Null Function, disable parser.
                    mSelected = false;
                }
                else
                {
                    mCurrentNumber.mLsb = inValue;
                    mSelected = mState.has(mCurrentNumber.as14bits());
                }
                break;

            case midi::DataIncrement:
                if (mSelected)
                {
                    Value& currentValue = getCurrentValue();
                    currentValue += inValue;
                    return true;
                }
                break;
            case midi::DataDecrement:
                if (mSelected)
                {
                    Value& currentValue = getCurrentValue();
                    currentValue -= inValue;
                    return true;
                }
                break;

            case midi::DataEntryMSB:
                if (mSelected)
                {
                    Value& currentValue = getCurrentValue();
                    currentValue.mMsb = inValue;
                    currentValue.mLsb = 0;
                    return true;
                }
                break;
            case midi::DataEntryLSB:
                if (mSelected)
                {
                    Value& currentValue = getCurrentValue();
                    currentValue.mLsb = inValue;
                    return true;
                }
                break;

            default:
                // Not part of the RPN/NRPN workflow, ignoring.
                break;
        }
        return false;
    }

public:
    inline Value& getCurrentValue()
    {
        return mState.get(mCurrentNumber.as14bits());
    }
    inline const Value& getCurrentValue() const
    {
        return mState.get(mCurrentNumber.as14bits());
    }

public:
    State& mState;
    bool mSelected;
    Value mCurrentNumber;
};

// --

typedef State<2> RpnState;  // We'll listen to 2 RPN
typedef State<4> NrpnState; // and 4 NRPN
typedef ParameterNumberParser<RpnState,  midi::RPNMSB,  midi::RPNLSB>  RpnParser;
typedef ParameterNumberParser<NrpnState, midi::NRPNMSB, midi::NRPNLSB> NrpnParser;

struct ChannelSetup
{
    inline ChannelSetup()
        : mRpnParser(mRpnState)
        , mNrpnParser(mNrpnState)
    {
    }

    inline void reset()
    {
        mRpnParser.reset();
        mNrpnParser.reset();
    }
    inline void setup()
    {
        mRpnState.enable(midi::RPN::PitchBendSensitivity);
        mRpnState.enable(midi::RPN::ModulationDepthRange);

        // Enable a few random NRPNs
        mNrpnState.enable(12);
        mNrpnState.enable(42);
        mNrpnState.enable(1234);
        mNrpnState.enable(1176);
    }

    RpnState    mRpnState;
    NrpnState   mNrpnState;
    RpnParser   mRpnParser;
    NrpnParser  mNrpnParser;
};

ChannelSetup sChannelSetup[16];

// --

void handleControlChange(byte inChannel, byte inNumber, byte inValue)
{
    ChannelSetup& channel = sChannelSetup[inChannel];

    if (channel.mRpnParser.parseControlChange(inNumber, inValue))
    {
        const Value& value    = channel.mRpnParser.getCurrentValue();
        const unsigned number = channel.mRpnParser.mCurrentNumber.as14bits();

        if (number == midi::RPN::PitchBendSensitivity)
        {
            // Here, we use the LSB and MSB separately as they have different meaning.
            const byte semitones    = value.mMsb;
            const byte cents        = value.mLsb;
        }
        else if (number == midi::RPN::ModulationDepthRange)
        {
            // But here, we want the full 14 bit value.
            const unsigned range = value.as14bits();
        }
    }
    else if (channel.mRpnParser.parseControlChange(inNumber, inValue))
    {
        // You get the idea..
    }
}

// --

void setup()
{
    for (int i = 0; i < 16; ++i)
    {
        ChannelSetup& channel = sChannelSetup[i];
        channel.reset();
        channel.setup();
    }
    MIDI.setHandleControlChange(handleControlChange);
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
    MIDI.read();

    // Send a RPN sequence (Pitch Bend sensitivity) on channel 1
    {
        const midi::Channel channel = 1;
        const byte semitones = 12;
        const byte cents     = 42;
        MIDI.beginRpn(midi::RPN::PitchBendSensitivity, channel);
        MIDI.sendRpnValue(semitones, cents, channel);
        MIDI.endRpn(channel);
    }
}
