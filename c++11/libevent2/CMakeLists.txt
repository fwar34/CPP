cmake_minimum_required(VERSION 3.10)
project(libevent2)

add_executable(main main.cpp Server.cpp Session.cpp)
add_compile_options(-Wall)

link_directories(/usr/local/lib /usr/lib)
include_directories(/usr/local/include /usr/include)
find_package(Libevent REQUIRED)
target_link_libraries(main event)