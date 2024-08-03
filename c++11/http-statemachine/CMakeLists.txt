cmake_minimum_required(VERSION 3.10)

project(http-statemachine)

set(CMAKE_BUILD_TYPE Debug)
set(SRC HttpParser.h Log.h HttpParser.cpp Log.cpp)

add_library(${PROJECT_NAME} SHARED ${SRC})
# PUBLIC属性指定了${PROJECT_NAME}包含的头文件目录暴露出去了，其他引用${PROJECT_NAME}的目标则继承此头文件目录（参考类的PUBLIC作用域）
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()
# PUBLIC属性指定了${PROJECT_NAME}所链接的库暴露出去了，其他引用${PROJECT_NAME}的目标则继承此链接库（参考类的PUBLIC作用域）
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

## 测试
add_subdirectory(test)