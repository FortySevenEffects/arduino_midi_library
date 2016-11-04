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
    tests/unit-tests_Settings.h
    tests/unit-tests_SysExCodec.cpp
    tests/unit-tests_SerialMock.cpp
    tests/unit-tests_MidiInput.cpp
    tests/unit-tests_MidiInputCallbacks.cpp
    tests/unit-tests_MidiOutput.cpp
    tests/unit-tests_MidiThru.cpp
    tests/unit-tests_MidiUsb.cpp
)

target_link_libraries(unit-tests
    gtest
    gmock
    midi
    test-mocks
)

add_test(unit-tests ${unit-tests_BINARY_DIR}/unit-tests --gtest_color=yes)
add_custom_target(build-and-run-unit-tests
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS unit-tests
)
