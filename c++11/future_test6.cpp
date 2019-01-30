/*************************************************************************
    > File Name: future_test6.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 PM02:02:28 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <chrono>

double task(int n)
{
    std::cout << std::this_thread::get_id()
        << " start computing..." << std::endl;

    double ret = 0;
    for (int i = 0; i <= n; ++i) {
        ret += std::sin(i);
    }

    std::cout << std::this_thread::get_id()
        << " finished computing..." << std::endl;
    return ret;
}

int main()
{
    std::future<double> fut(std::async(std::launch::async, task, 100000000));
#if 0
    while (fut.wait_until(std::chrono::system_clock::now() + std::chrono::seconds(1)) !=
           std::future_status::ready) {
        std::cout << "task is running..." << std::endl;
    }
#else
    while (fut.wait_for(std::chrono::seconds(1)) !=
           std::future_status::ready) {
        std::cout << "task is running..." << std::endl;
    }
#endif

    std::cout << fut.get() << std::endl;

    return 0;
}
