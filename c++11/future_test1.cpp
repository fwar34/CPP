/*************************************************************************
    > File Name: future_test1.cpp
    > Author: Feng
    > Created Time: Tuesday, January 29, 2019 PM03:40:09 CST
    > Content: http://www.cnblogs.com/haippy/p/3279565.html
 ************************************************************************/

#include <iostream>
#include <future>
#include <thread>
#include <functional>
#include <chrono>

void task(std::future<int>& fut)
{
    std::cout << "task start\n";
    int x = fut.get();
    std::cout << "value: " << x << std::endl;
}

int main()
{
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread t(task, std::ref(fut));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    prom.set_value(10);
    t.join();

    return 0;
}
