/*************************************************************************
    > File Name: condition_variable_test3.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 PM05:10:24 CST
    > Content: 有可能两个producer启动了，而这时候还没有consumer，就导致了丢信号
    >       http://www.cnblogs.com/haippy/p/3252041.html
 ************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <condition_variable>

std::mutex g_mtx;
std::condition_variable g_cv;

int cargo = 0;
void consumer()
{
    std::unique_lock<std::mutex> lck(g_mtx);
    while (cargo == 0) {
        g_cv.wait(lck);
    }

    std::cout << cargo << std::endl;
    cargo = 0;
}

void producer(int i)
{
    std::unique_lock<std::mutex> lck(g_mtx);
    cargo = i;
    std::cout << "producer " << i << std::endl;
    g_cv.notify_one();
}

int main()
{
    std::vector<std::thread> consumers;
    std::vector<std::thread> producers;
    for (int i = 0; i < 10; ++i) {
        consumers.emplace_back(std::thread(consumer));
        producers.emplace_back(std::thread(producer, i + 1));
    }

    std::for_each(producers.begin(), producers.end(), std::mem_fn(&std::thread::join));
    std::for_each(consumers.begin(), consumers.end(), std::mem_fn(&std::thread::join));

    return 0;
}
