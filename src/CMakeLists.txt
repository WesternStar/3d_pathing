cmake_minimum_required(VERSION 3.13)
project(3dPaths)
include_directories(${CMAKE_SOURCE_DIR}/../include)
add_executable(pgmTest pgmTest.cpp)
add_executable(2dPathing main2D.cpp distance2D.cpp)
add_executable(3dPathing main.cpp distance.cpp)
add_executable(box box.cpp)
set_property(TARGET box PROPERTY CXX_STANDARD 17)
set_property(TARGET 2dPathing PROPERTY CXX_STANDARD 17)
set_property(TARGET 3dPathing PROPERTY CXX_STANDARD 17)
set_property(TARGET pgmTest PROPERTY CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
