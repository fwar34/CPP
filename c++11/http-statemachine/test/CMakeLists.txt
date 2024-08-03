cmake_minimum_required(VERSION 3.10)

project(http-test)
message(${CMAKE_BUILD_TYPE})

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} http-statemachine)

# 面向过程的例子
add_executable(main main.cpp)