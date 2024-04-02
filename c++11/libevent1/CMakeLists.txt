cmake_minimum_required(VERSION 3.10)

project(libevent1)

# find_package(Libevent REQUIRED COMPONENTS ${EVENT__LINK_COMPONENT})
find_package(Libevent REQUIRED)

link_directories(
    /usr/local/lib
    /usr/lib
)

include_directories(
    /usr/local/include
    /usr/include
)

add_compile_options(-Wall)

add_executable(main main.cpp ThreadPool.h Server.cpp Session.cpp SessionMgr.cpp)
# add_executable(helloworld helloworld.cpp)

# target_link_libraries(main ${LIBEVENT_LIBRARIES})
target_link_libraries(main event)
# target_link_libraries(helloworld event)
