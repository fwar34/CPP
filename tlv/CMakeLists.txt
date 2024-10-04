cmake_minimum_required(VERSION 3.10)

project(tlv)

set(CMAKE_BUILD_TYPE Debug)

add_library(tlv tlv.c tlv.h)

# test
add_executable(test test.c)
target_link_libraries(test tlv)