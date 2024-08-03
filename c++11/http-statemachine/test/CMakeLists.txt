cmake_minimum_required(VERSION 3.10)

project(http-test)

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} http-statemachine)