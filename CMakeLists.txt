cmake_minimum_required(VERSION 3.3)
project(RaxSimulate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Model.cpp Model.hpp range.hpp range.hpp Event_Queue.hpp Event_Queue.cpp Event.cpp Event.hpp)
add_executable(RaxSimulate ${SOURCE_FILES})