/*************************************************************************
    > File Name: atomic_test5.cpp
    > Author: Feng
    > Created Time: Thursday, January 31, 2019 PM02:43:09 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>

std::atomic<bool> lock(false);
std::atomic<bool> ready(false);

void task(int x)
{
    while (!ready) {
    }

    for (int i = 0; i < 1000000; ++i) {
    }

    if (!lock.exchange(true)) {
        std::cout << "thread #" << x << " won" << std::endl;
    }
}

int main()
{
    std::vector<std::thread> ts;
    for (int i = 0; i < 10; ++i) {
        ts.emplace_back(task, i);
    }
    ready.store(true);

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    return 0;
}
