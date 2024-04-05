cmake_minimum_required(VERSION 3.10)
project(libevent2)

# It's 21 century!
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ImportLibrary)

# download libevent
set(EVENT__LIBRARY_TYPE "STATIC")
import_library(
  libevent
  https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
  b5333f021f880fe76490d8a799cd79f4)
add_subdirectory(${IMPORT_SRC} ${IMPORT_BUILD} EXCLUDE_FROM_ALL)

add_executable(main main.cpp Server.cpp Session.cpp 
    IOServicePool.cpp SessionMgr.cpp CommonDefine.cpp Signal.cpp
    IOThread.cpp Object.cpp Reactor.cpp Conference.cpp User.cpp
    ConferenceMgr.cpp Message.cpp SendQueue.cpp)
add_compile_options(-Wall)
# add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

link_directories(/usr/local/lib /usr/lib)
include_directories(/usr/local/include /usr/include)
# find_package(Libevent REQUIRED)
target_link_libraries(main event_core)