/*************************************************************************
    > File Name: chrono_test3.cpp
    > Author: Feng
    > Created Time: 2020-06-19 11:03
    > Content: 获取当前的系统时间戳
 ************************************************************************/

#include <iostream>
#include <chrono>

int main()
{
    std::chrono::time_point<std::chrono::steady_clock> timestamp = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count() << std::endl;
    return 0;
}

