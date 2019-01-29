/*************************************************************************
    > File Name: lock_test4.cpp
    > Author: Feng
    > Created Time: Tuesday, January 29, 2019 AM10:22:18 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

std::timed_mutex g_mtx;
void fireworks(int i)
{
    std::unique_lock<std::timed_mutex> lck(g_mtx, std::defer_lock);
    while (!lck.try_lock_for(std::chrono::milliseconds(100))) {
        std::cout << '-' << i;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "*" << i << std::endl;
}

int main()
{
    std::vector<std::thread> ts;
    for (int i = 0; i < 10; ++i) {
        ts.emplace_back(std::thread(fireworks, i));
    }

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    return 0;
}
