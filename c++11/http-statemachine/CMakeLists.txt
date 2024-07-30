cmake_minimum_required(VERSION 3.10)

project(http-statemachine)

set(SRC HttpParser.cpp)

add_library(http-statemachine SHARED HttpParser.h ${SRC})
add_executable(test test.cpp ${SRC})