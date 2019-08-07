cmake_minimum_required(VERSION 3.2)
project(NESemu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(NESemu ${SOURCE_FILES} CPU6502.cpp CPU6502.h RAM.cpp RAM.h Cartridge.cpp Cartridge.h)