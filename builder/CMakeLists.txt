
macro(setup_builder)
    enable_testing()

    set(ROOT_SOURCE_DIR ${PROJECT_SOURCE_DIR} CACHE INTERNAL "Repository root directory")
    set(ROOT_BINARY_DIR "${ROOT_SOURCE_DIR}/build")

    include_directories(${ROOT_SOURCE_DIR})

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

endmacro()
