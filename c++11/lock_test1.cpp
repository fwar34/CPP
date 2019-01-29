/*************************************************************************
    > File Name: lock_test1.cpp
    > Author: Feng
    > Created Time: Monday, January 28, 2019 PM05:20:24 CST
    > Content: https://www.cnblogs.com/xudong-bupt/p/9194394.html
 ************************************************************************/

#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>
#include <vector>

std::mutex g_mtx;
void add(int& num, int& sum)
{
    while (true) {
        std::lock_guard<std::mutex> guard(g_mtx);
        if (num < 100) {
            num++;
            sum += num;
        } else {
            break;
        }
    }
}

int main()
{
    int sum = 0;
    int num = 0;
    std::vector<std::thread> ts;
    for (int i = 0; i < 20; ++i) {
        ts.emplace_back(std::thread(add, std::ref(num), std::ref(sum)));
        //std::thread t = std::thread(add, std::ref(num), std::ref(sum));
        //ts.emplace_back(std::move(t));
    }

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    std::cout << sum << std::endl;

    return 0;
}
