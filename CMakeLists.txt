cmake_minimum_required(VERSION 3.6)
project(exedump)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_executable(exedump src/main.cpp src/PortableExecutable.hpp src/PortableExecutable.cpp)
target_link_libraries(exedump LINK_PUBLIC Std++ Std++Static)