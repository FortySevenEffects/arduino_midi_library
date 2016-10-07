include(CMakeToolsHelpers OPTIONAL)

project(unit-tests)

include_directories(
    ${unit-tests_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)

add_executable(unit-tests

    unit-tests.cpp
    unit-tests.h
    unit-tests_Namespace.h

    tests/unit-tests_MidiMessage.cpp
    tests/unit-tests_Settings.cpp
    tests/unit-tests_SysExCodec.cpp
    tests/unit-tests_SerialMock.cpp
    tests/unit-tests_MidiInput.cpp
    tests/unit-tests_MidiOutput.cpp
    tests/unit-tests_MidiThru.cpp
)

target_link_libraries(unit-tests
    gtest
    gmock
    midi
    test-mocks
)

option(RUN_UNIT_TESTS_IN_POST_BUILD false)

if (RUN_UNIT_TESTS_IN_POST_BUILD)
    add_custom_command(TARGET unit-tests POST_BUILD
        COMMAND ${unit-tests_BINARY_DIR}/unit-tests
    )
endif()
