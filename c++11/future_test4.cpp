/*************************************************************************
    > File Name: future_test4.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 AM11:21:15 CST
    > Content: http://www.cnblogs.com/haippy/p/3280643.html
 ************************************************************************/

#include <iostream>
#include <chrono>
#include <thread>
#include <future>

bool is_prime(int x)
{
    for (int i = 2; i < x; ++i) {
        if (x % 2 == 0) {
            return false;
        }
    }
    return true;
}

int main()
{
    int num = 4444445;
    std::future<bool> fut = std::async(is_prime, num);
    std::cout << "checking, wait..." << std::endl;
    std::chrono::milliseconds span(100);
    while (fut.wait_for(span) == std::future_status::timeout) {
        std::cout << '-';
    }

    bool x = fut.get();
    std::cout << num << (x ? " is" : " is not") << " prime" << std::endl;
    return 0;
}
