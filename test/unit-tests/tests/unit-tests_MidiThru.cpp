#include "unit-tests.h"
#include "unit-tests_Settings.h"
#include <src/MIDI.h>
#include <test/mocks/test-mocks_SerialMock.h>

BEGIN_MIDI_NAMESPACE

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
USING_NAMESPACE_UNIT_TESTS
typedef test_mocks::SerialMock<32> SerialMock;
typedef midi::MidiInterface<SerialMock> MidiInterface;

template<unsigned Size>
struct VariableSysExSettings : midi::DefaultSettings
{
    static const unsigned SysExMaxSize = Size;
};

// -----------------------------------------------------------------------------

TEST(MidiThru, defaultValues)
{
    SerialMock serial;
    MidiInterface midi(serial);

    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.begin(); // Should not change the state
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
}

TEST(MidiThru, beginEnablesThru)
{
    SerialMock serial;
    MidiInterface midi(serial);

    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);
    midi.begin();
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
}

TEST(MidiThru, setGet)
{
    SerialMock serial;
    MidiInterface midi(serial);

    midi.turnThruOff();
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);

    midi.turnThruOn();
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.turnThruOn(midi::Thru::SameChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::SameChannel);
    midi.turnThruOn(midi::Thru::DifferentChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::DifferentChannel);

    midi.setThruFilterMode(midi::Thru::Full);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Full);
    midi.setThruFilterMode(midi::Thru::SameChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::SameChannel);
    midi.setThruFilterMode(midi::Thru::DifferentChannel);
    EXPECT_EQ(midi.getThruState(),  true);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::DifferentChannel);
    midi.setThruFilterMode(midi::Thru::Off);
    EXPECT_EQ(midi.getThruState(),  false);
    EXPECT_EQ(midi.getFilterMode(), midi::Thru::Off);
}

END_UNNAMED_NAMESPACE
