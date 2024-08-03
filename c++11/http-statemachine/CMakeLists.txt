cmake_minimum_required(VERSION 3.10)

project(http-statemachine)

set(SRC HttpParser.cpp)

add_library(http-statemachine SHARED HttpParser.h ${SRC})
target_include_directories(http-statemachine PUBLIC ${CMAKE_SOURCE_DIR})

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()
target_link_libraries(http-statemachine PUBLIC spdlog::spdlog)

## 测试用
add_executable(test test.cpp ${SRC})