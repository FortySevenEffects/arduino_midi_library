project(test-runner)

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(test-runner
    test-runner.cpp
    test-runner.h
    test-runner_Namespace.h
    test-runner_Main.cpp
    test-runner_Main.h
)
target_link_libraries(test-runner
    test-mocks
    gtest
)
