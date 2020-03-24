project(test-mocks)

add_library(test-mocks STATIC
    RingBuffer.h
    RingBuffer.hpp
    RingBuffer.cpp
    test-mocks.cpp
    test-mocks.h
    test-mocks_Namespace.h
    test-mocks_SerialMock.cpp
    test-mocks_SerialMock.hpp
    test-mocks_SerialMock.h
)

target_link_libraries(test-mocks
    midi
)
