## Requirements

Requirements to build and run the unit tests:
- CMake 2.8 or later
- GCC / Clang with C++11 support

## Setup

Pull Google Test / Google Mock subrepository:
```
$ git init submodules
```

Create build directory, run CMake, build and run unit tests:
```
$ mkdir build && cd build
$ cmake ..
$ make
$ ctest --verbose
```
