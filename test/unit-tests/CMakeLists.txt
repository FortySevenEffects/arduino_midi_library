project(unit-tests)

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(unit-tests

    unit-tests.cpp
    unit-tests.h
    unit-tests_Namespace.h
    unit-tests_MidiMessage.cpp
)

target_link_libraries(unit-tests
    gtest
    midi
    test-mocks
)

add_custom_command(TARGET unit-tests POST_BUILD
    COMMAND ${unit-tests_BINARY_DIR}/unit-tests
)

