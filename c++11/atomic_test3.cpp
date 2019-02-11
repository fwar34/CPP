/*************************************************************************
    > File Name: atomic_test3.cpp
    > Author: Feng
    > Created Time: Thursday, January 31, 2019 PM02:01:04 CST
    > Content: 结合 std::atomic_flag::test_and_set() 和 
    >          std::atomic_flag::clear()，std::atomic_flag 对象可以当作一个简单的自旋锁使用
    >          http://www.cnblogs.com/haippy/p/3252056.html
 ************************************************************************/

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>

std::atomic_flag lock = ATOMIC_FLAG_INIT;

void f(int n)
{
    for (int i = 0; i < 100; ++i) {
        while (lock.test_and_set(std::memory_order_acquire)) 
            ;
        std::cout << "Output from thread " << n << '\n';
        lock.clear(std::memory_order_release);
    }
}

int main()
{
    std::vector<std::thread> ts;
    for (int i = 0; i < 10; ++i) {
        ts.emplace_back(f, i);
    }

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    return 0;
}
