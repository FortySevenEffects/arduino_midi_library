cmake_minimum_required(VERSION 3.3.0)
project(arduino_midi_library)

set(ROOT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${ROOT_SOURCE_DIR})

add_subdirectory(src)
add_subdirectory(test)
