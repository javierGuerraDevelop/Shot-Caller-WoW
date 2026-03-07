cmake_minimum_required(VERSION 3.10)

project(ShotCallerWow)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ShotCallerWow src/main.cpp src/parser.cpp src/line_reader.cpp src/engine.cpp src/line_writer.cpp src/socket_sender.cpp)

target_include_directories(ShotCallerWow PUBLIC include)
