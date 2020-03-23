#pragma once

#include "unit-tests.h"
#include <src/midi_Platform.h>

BEGIN_UNIT_TESTS_NAMESPACE

struct VariablePlatform : public midi::DefaultPlatform
{
};

END_UNIT_TESTS_NAMESPACE
