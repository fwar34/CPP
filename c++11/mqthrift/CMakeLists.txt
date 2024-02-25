cmake_minimum_required(VERSION 3.10)

project(mqthrift)

set(SRC main.cpp MqThrift.cpp MqThriftImpl.cpp MqThriftProducer.cpp MqThriftConsumer.cpp)

add_executable(main ${SRC})