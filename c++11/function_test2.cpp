/*************************************************************************
    > File Name: function_test1.cpp
    > Author: Feng
    > Created Time: 2019-07-19 15:56
    > Content: function switch thread
************************************************************************/

#include <iostream>
#include <functional>
#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <sys/time.h>

std::condition_variable cond;
std::mutex mtx;
bool stop = false;

std::list<std::function<void()>> fifo;

#if 0
template<typename _Clock, typename _Duration, typename _Predicate>
bool
wait_until(unique_lock<mutex>& __lock,
           const chrono::time_point<_Clock, _Duration>& __atime,
           _Predicate __p)
{
    while (!__p())
        if (wait_until(__lock, __atime) == cv_status::timeout)
            return __p();
    return true;
}
#endif

void process_wait_for(std::list<std::function<void()>>& fifo)
{
    while (!stop) {
        std::cv_status status;
        std::unique_lock<std::mutex> lock(mtx);
        auto before = std::chrono::steady_clock::now();
        while (fifo.empty()) {
            // bool ret = cond.wait_until(lock, std::chrono::steady_clock::now() + std::chrono::seconds(1),
            //                            [&fifo] () { return !fifo.empty(); });
            std::cv_status status = cond.wait_until(lock, before + std::chrono::milliseconds(500));
            if (status == std::cv_status::timeout) {
                break;
            }
        }

        // for (auto it = fifo.begin(); it != fifo.end(); ) {
        //     auto& task = *it;
        //     task();
        //     fifo.erase(it++);
        // }

        // struct timeval tv;
        // gettimeofday(&tv, nullptr);
        // std::cout << "handle input: time: " << tv.tv_sec * 1000 + tv.tv_usec / 1000 << std::endl;
        auto input_time = std::chrono::system_clock::now();
        std::cout << "handle input: time: " << std::chrono::system_clock::to_time_t(input_time) << std::endl;

        std::list<std::function<void()>> ready_task_list;
        ready_task_list = fifo;
        fifo.clear();
        lock.unlock();

        for (auto& task : ready_task_list) {
            std::cout << "thread task address: " << &task << std::endl;
            task();
        }

        // 判断定时器时间是否到达，如果已经到达则触发定时器
        auto after = std::chrono::steady_clock::now();
        if (status == std::cv_status::timeout || after > before + std::chrono::milliseconds(500)) {
            // gettimeofday(&tv, nullptr);
            // std::cout << "timeout: time: " << tv.tv_sec * 1000 + tv.tv_usec / 1000 << std::endl;
            auto timeout_time = std::chrono::system_clock::now();
            std::cout << "timeout: time: " << std::chrono::system_clock::to_time_t(timeout_time) << std::endl;
        }
    }
    std::cout << "thread stop..." << std::endl;
}

void process(std::list<std::function<void()>>& fifo)
{
    while (!stop) {
        std::unique_lock<std::mutex> lock(mtx);
        // 1.
        // while (fifo.empty()) {
        //     cond.wait(lock);
        // }
        // 2.
        // cond.wait(lock, [] () { return !fifo.empty(); })
        // 1.和2.实际上等价, c++11在b内部循环来wait，防止A.提前唤醒B.假唤醒
        cond.wait(lock, [&fifo] () { return !fifo.empty(); });
        std::list<std::function<void()>> ready_task_list;
        ready_task_list = fifo;
        fifo.clear();
        lock.unlock();

        for (auto& task : ready_task_list) {
            std::cout << "thread task address: " << &task << std::endl;
            task();
        }
    }
    std::cout << "thread stop..." << std::endl;
}

void test()
{
    // std::cout << "test" << std::endl;
}

struct data {
    int a = 10;
    int b = 20;
};

int main()
{
    // std::thread t(process, std::ref(fifo));
    std::thread t(process_wait_for, std::ref(fifo));

    data da;
    std::cout << "main data address: " << &da << std::endl;
    std::function<void()> task = std::bind([](data& da) {
                                               std::ostringstream oss;
                                               // oss << "------------------" << std::endl;
                                               oss << "thread data address: " << &da << std::endl;
                                               // oss << da.a << std::endl;
                                               // oss << da.b << std::endl;
                                               // oss << "------------------" << std::endl;
                                               std::cout << oss.str() << std::endl;
                                           }, da); //bind 函数拷贝data
    std::cout << "main task address: " << &task << std::endl;

    // for (int i = 0; i < 300; ++i)
    while (true)
    {
        {
            std::lock_guard<std::mutex> cond_lock(mtx);
            // bool notify = fifo.empty();
            fifo.push_back(task); //push bind function
            fifo.push_back(std::bind([](int x) {
                                         // std::cout << x << std::endl;
                                     }, 5555)); //push bind function
            fifo.push_back(test); //push c function
            // if (notify) { //send condition variable when fifo empty
            //     cond.notify_all();
            // } 

            // 这里不用判空来notify，如果内部没有wait好像不会进入系统api
            cond.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    t.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    //stop thread
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
        fifo.push_back([](){});
        cond.notify_all();
        std::cout << "send stop" << std::endl;
    }
    // t.join();

    return 0;
}
