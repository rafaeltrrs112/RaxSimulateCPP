cmake_minimum_required(VERSION 3.3)
project(RaxSimulate)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Main.cpp range.hpp range.hpp Event_Queue.hpp Event_Queue.cpp Event.cpp Event.hpp BinaryRouter.hpp Input_Event.hpp OutPut_Event.hpp Token.hpp Processable_Token.hpp Model.cpp Model.hpp State.hpp Part_Three.cpp PartTwo.cpp XorToken.cpp XorToken.hpp XorTokenBag.cpp XorTokenBag.hpp XorState.cpp XorState.hpp XorModel.cpp XorModel.hpp RouteState.cpp RouteState.hpp PassThroughRouter.cpp PassThroughRouter.hpp MemoryState.cpp MemoryState.hpp MemoryModule.cpp MemoryModule.hpp Log.cpp Log.hpp KickOffEvent.cpp KickOffEvent.hpp PartOne.cpp)
add_executable(RaxSimulate ${SOURCE_FILES})