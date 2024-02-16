#include "SyncQueue2.h"
#include "ThreadPool2.h"
#include "ThreadPool3.h"
#include <iostream>
#include <chrono>

void TestThreadPool2()
{
    ThreadPool2 pool2(10);
    auto ret = pool2.PushTask([](int x) -> int {
        std::cout << "thread id = " << std::this_thread::get_id() << " x = " << x << std::endl;
        return ++x;
    }, 334);

    std::cout << ret.get() << std::endl;
}

void TestThreadPool3()
{
    ThreadPool3 pool3;
    pool3.PushTask(std::bind([] (int x) -> int {
        std::cout << "thread x = " << x << std::endl;
        return ++x;
    }, 334));
    pool3.PushTask([] {
        std::cout << "ani2" << std::endl;
    });

    auto func = std::bind([] (int x) {
        std::cout << "lvaule" << std::endl;
        return x++;
    }, 443);
    pool3.PushTask(func);
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    pool3.Stop();
}

int main(int argc, char const *argv[])
{
    // SyncQueue2<int> queue(10);
    // int a = 33;
    // queue.Push(a);
    // queue.Push(44);

    TestThreadPool2();
    TestThreadPool3();

    return 0;
}
