#include "unit-tests_Settings.h"

BEGIN_MIDI_NAMESPACE

const bool DefaultSettings::UseRunningStatus;
const bool DefaultSettings::HandleNullVelocityNoteOnAsNoteOff;
const bool DefaultSettings::Use1ByteParsing;
const unsigned DefaultSettings::SysExMaxSize;

END_MIDI_NAMESPACE

// -----------------------------------------------------------------------------

BEGIN_UNNAMED_NAMESPACE

TEST(Settings, hasTheRightDefaultValues)
{
    EXPECT_EQ(MIDI_NAMESPACE::DefaultSettings::UseRunningStatus,                   false);
    EXPECT_EQ(MIDI_NAMESPACE::DefaultSettings::HandleNullVelocityNoteOnAsNoteOff,  true);
    EXPECT_EQ(MIDI_NAMESPACE::DefaultSettings::Use1ByteParsing,                    true);
    EXPECT_EQ(MIDI_NAMESPACE::DefaultSettings::SysExMaxSize,                       unsigned(128));
}

END_UNNAMED_NAMESPACE
