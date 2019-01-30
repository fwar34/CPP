/*************************************************************************
    > File Name: condition_variable_test1.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 PM02:22:10 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>

std::mutex g_mtx;
std::condition_variable g_cv;
bool g_ready = false;

void print(int id)
{
    std::unique_lock<std::mutex> lck(g_mtx);
    while (!g_ready) {
        g_cv.wait(lck);
    }
    std::cout << "thread " << id << std::endl;
}

void go()
{
    std::unique_lock<std::mutex> lck(g_mtx);
    g_ready = true;
    g_cv.notify_all();
}

int main()
{
    std::vector<std::thread> ts;
    for (int i = 0; i < 10; ++i) {
        ts.emplace_back(std::thread(print, i));
    }

    std::cout << "10 threads ready to race...\n";
    go();

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    return 0;
}
