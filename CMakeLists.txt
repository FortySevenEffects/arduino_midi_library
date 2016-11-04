cmake_minimum_required(VERSION 2.8.7)
project(arduino_midi_library CXX)

add_subdirectory(builder)

setup_builder()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)
