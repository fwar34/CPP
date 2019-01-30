/*************************************************************************
    > File Name: future_test3.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 AM11:00:43 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <chrono>
#include <thread>
#include <future>

std::future<int> launcher(std::packaged_task<int(int)>& task, int arg)
{
    if (task.valid()) {
        std::future<int> fut = task.get_future();
        std::thread(std::move(task), arg).detach();
        return fut;
    } else {
        return std::future<int>();
    }
}

int main()
{
    std::packaged_task<int(int)> task([](int x){ return x * 2; });
    std::future<int> fut = launcher(task, 25);
    std::cout << "The double of 25 is " << fut.get() << std::endl;

    return 0;
}
