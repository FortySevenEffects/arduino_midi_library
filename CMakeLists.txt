cmake_minimum_required(VERSION 2.8.7)
project(arduino_midi_library)

set(ROOT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${ROOT_SOURCE_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)
