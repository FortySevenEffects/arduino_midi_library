#pragma once

#include "unit-tests.h"
#include <src/midi_Settings.h>

BEGIN_UNIT_TESTS_NAMESPACE

template<bool RunningStatus, bool OneByteParsing>
struct VariableSettings : public midi::DefaultSettings
{
    static const bool UseRunningStatus = RunningStatus;
    static const bool Use1ByteParsing  = OneByteParsing;
};

template<bool A, bool B>
const bool VariableSettings<A, B>::UseRunningStatus;
template<bool A, bool B>
const bool VariableSettings<A, B>::Use1ByteParsing;

END_UNIT_TESTS_NAMESPACE
