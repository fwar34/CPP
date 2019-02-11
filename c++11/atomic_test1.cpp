/*************************************************************************
    > File Name: atomic_test1.cpp
    > Author: Feng
    > Created Time: Thursday, January 31, 2019 PM12:05:19 CST
    > Content: http://www.cnblogs.com/haippy/p/3252056.html
 ************************************************************************/

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>

std::atomic<bool> g_ready(false);
std::atomic_flag winner = ATOMIC_FLAG_INIT;

void count1m(int id)
{
    // 等待主线程中设置 ready 为 true.
    while (!g_ready) {
        std::this_thread::yield();
    }

    // 计数.
    for (int i = 0; i < 1000000; ++i) {
    }

    // 如果某个线程率先执行完上面的计数过程，则输出自己的 ID.
    // 此后其他线程执行 test_and_set 是 if 语句判断为 false，
    // 因此不会输出自身 ID.
    if (!winner.test_and_set()) {
        std::cout << "thread #" << id << " won" << std::endl;
    }
}

int main()
{
    std::vector<std::thread> ts;
    std::cout << "spawning 10 threads that count to 1 million...\n";
    for (int i = 0; i < 10; ++i) {
#if 0
        ts.emplace_back(std::thread(count1m, i + 1));
#else
        ts.emplace_back(count1m, i + 1);
#endif
    }
    g_ready = true;

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    return 0;
}
