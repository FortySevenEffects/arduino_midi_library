# Contributing Guidelines

First, thanks for your help ! :+1:

## Branches

Please base your Pull Requests off the `master` branch.

## Requirements

Requirements to build and run the unit tests:

-   CMake 2.8 or later
-   GCC / Clang with C++11 support (GCC 4.8 or higher)

## Setup

Pull Google Test / Google Mock subrepository:

```
$ git submodule init
$ git submodule update
```

Create build directory, run CMake, build and run unit tests:

```
$ mkdir build && cd build
$ cmake ..
$ make
$ ctest --verbose
```
