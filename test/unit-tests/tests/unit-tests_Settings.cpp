#include "unit-tests_Settings.h"

BEGIN_MIDI_NAMESPACE

const bool AbstractDefaultSettings::UseRunningStatus;
const bool AbstractDefaultSettings::HandleNullVelocityNoteOnAsNoteOff;
const bool AbstractDefaultSettings::Use1ByteParsing;
const unsigned AbstractDefaultSettings::SysExMaxSize;

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

TEST(Settings, hasTheRightDefaultValues)
{
    EXPECT_EQ(midi::AbstractDefaultSettings::UseRunningStatus,                   false);
    EXPECT_EQ(midi::AbstractDefaultSettings::HandleNullVelocityNoteOnAsNoteOff,  true);
    EXPECT_EQ(midi::AbstractDefaultSettings::Use1ByteParsing,                    true);
    EXPECT_EQ(midi::AbstractDefaultSettings::SysExMaxSize,                       unsigned(128));
}

END_UNNAMED_NAMESPACE
