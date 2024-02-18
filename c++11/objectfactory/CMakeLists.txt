cmake_minimum_required(VERSION 3.10)

project(ObjectFactory)
# set(CMAKE_CXX_STANDARD 14)
# message(STATUS "c++ standard = ${CMAKE_CXX_STANDARD}")

add_executable(main main.cpp ObjectFactory.cpp)