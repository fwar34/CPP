/*************************************************************************
    > File Name: future_test2.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 AM10:47:20 CST
    > Content: http://www.cnblogs.com/haippy/p/3279565.html
 ************************************************************************/

#include <iostream>
#include <thread>
#include <future>
#include <chrono>

int countdown(int from, int to)
{
    for (int i = from; i != to; --i) {
        std::cout << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "countdown finish" << std::endl;
    return from - to;
}

int main()
{
    std::packaged_task<int(int, int)> task(countdown);
    std::future<int> fut = task.get_future();
    std::thread t(std::move(task), 10, 0);
    int value = fut.get();
    std::cout << "The countdown last for " << value << std::endl;
    t.join();
    return 0;
}
