cmake_minimum_required(VERSION 3.10)

project(endpoint)

add_executable(${PROJECT_NAME} main.cpp)

# find_package(Boost REQUIRED COMPONENTS asio)

# include_directories(${Boost_INCLUDE_DIRS})
# target_link_libraries(main ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})