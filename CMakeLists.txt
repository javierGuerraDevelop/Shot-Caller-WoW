cmake_minimum_required(VERSION 3.14)

project(ShotCallerWow)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ShotCallerWow src/main.cpp src/parser.cpp src/line_reader.cpp src/engine.cpp src/line_writer.cpp src/socket_sender.cpp)

target_include_directories(ShotCallerWow PUBLIC include)

# Static library for testable modules (no I/O)
add_library(ShotCallerLib STATIC src/parser.cpp src/engine.cpp)
target_include_directories(ShotCallerLib PUBLIC include)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(ShotCallerTests
    tests/test_constants.cpp
    tests/test_parser.cpp
    tests/test_engine.cpp
)
target_link_libraries(ShotCallerTests ShotCallerLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ShotCallerTests)
