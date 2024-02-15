cmake_minimum_required(VERSION 3.10)

project(ThreadPool2)

add_executable(main main.cpp SyncQueue2.h ThreadPool2.h ThreadPool3.h)