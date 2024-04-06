cmake_minimum_required(VERSION 3.10)

project(protobuf-test)

# 查找并添加 protobuf 库，并且为必选项
find_package(Protobuf REQUIRED)
# 将 Protobuf 库的头文件路径添加到搜索路径中
include_directories(${PROTOBUF_INCLUDE_DIRS})

# 创建一个名为 person_proto 的库，包含 person.pb.cc 文件
# add_library(person_x, person.pb.cc)
# 将 Protobuf 库链接到 person_x 库
# target_link_libraries(person_x, ${PROTOBUF_LIBRARIES})

set(EXECUTABLE_SRCS main.cpp)

# 创建一个名为main的可执行文件，包含EXECUTABLE_SRCS中指定的源代码文件
add_executable(main ${EXECUTABLE_SRCS} person.pb.cc)
# 将 person_x 库链接到 main 可执行文件
target_link_libraries(main ${PROTOBUF_LIBRARIES})

# 设置 C++ 标准为 C++14
set(CMAKE_CXX_STANDARD 14)
# 要求编译器必须支持 C++14 标准
set(CMAKE_CXX_STANDARD_REQUIRED True)

