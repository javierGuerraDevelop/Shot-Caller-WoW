cmake_minimum_required(VERSION 3.10)

project(shot_caller_cpp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Define the executable target.
add_executable(shot_caller_cpp src/main.cpp src/parser.cpp src/line_reader.cpp
                             src/engine.cpp)

# 1. Configure properties for that target.
target_include_directories(shot_caller_cpp PUBLIC include)
