/*************************************************************************
    > File Name: lock_test2.cpp
    > Author: Feng
    > Created Time: Tuesday, January 29, 2019 AM09:56:02 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>

std::mutex g_mtx;

void print_thread_id(int id)
{
    g_mtx.lock();
    std::lock_guard<std::mutex> guard(g_mtx, std::adopt_lock);
    std::cout << "thread #" << id << std::endl;
}

int main()
{
    std::thread ts[10];
    for (int i =0; i < 10; ++i) {
        ts[i] = std::thread(print_thread_id, i + 1);
    }

    for (auto& t : ts) {
        t.join();
    }

    return 0;
}
