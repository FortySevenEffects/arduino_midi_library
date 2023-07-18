cmake_minimum_required(VERSION 3.26.4)
project(arduino_midi_library CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(builder)

setup_builder()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)
