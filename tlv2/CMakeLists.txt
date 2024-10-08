cmake_minimum_required(VERSION 3.10)

project(tlv)

set(CMAKE_BUILD_TYPE Debug)

add_library(tlv tlv.c tlv.h)

# test
add_executable(client client.c command.h common.h)
target_link_libraries(client tlv)

add_executable(server server.c buffer.c command.h common.h)
target_link_libraries(server tlv)