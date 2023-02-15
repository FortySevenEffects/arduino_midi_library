include(CMakeToolsHelpers OPTIONAL)

project(analyzer)

include_directories(
    "${analyzer_SOURCE_DIR}"
)

add_executable(analyzer
    analyzer.cpp
    analyzer.h
    analyzer_Namespace.h

    analyzer_MIDI.cpp
    analyzer_MIDI.h
)

target_link_libraries(analyzer
    midi
    test-mocks
)

