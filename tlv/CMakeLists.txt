cmake_minimum_required(VERSION 3.10)

add_library(tlv tlv.c tlv.h)

# test
add_executable(test test.c)
target_link_libraries(tlv)