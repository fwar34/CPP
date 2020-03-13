/*************************************************************************
    > File Name: function_test1.cpp
    > Author: Feng
    > Created Time: 2019-07-19 15:56
    > Content: C++11实现的工作线程切换和工作线程固定时间的定时器触发(wait_for, wait_until)
               以及线程间切换使用function
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
#include <unistd.h>

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
    using clock = std::chrono::steady_clock;
    auto last_timeout = clock::now();
    
    while (!stop) {
        std::cv_status status;
        std::unique_lock<std::mutex> lock(mtx);
        auto t1 = clock::now();
        while (fifo.empty()) {
            status = cond.wait_for(lock, std::chrono::milliseconds(400));
            if (status == std::cv_status::timeout) {
                break;
            }
        }

        // for (auto it = fifo.begin(); it != fifo.end(); ) {
        //     auto& task = *it;
        //     task();
        //     fifo.erase(it++);
        // }

        if (status != std::cv_status::timeout) {
            auto input_time = std::chrono::system_clock::now();
            std::cout << "handle input: time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(input_time.time_since_epoch()).count()
                      << std::endl;
        }

        std::list<std::function<void()>> ready_task_list;
        ready_task_list = fifo;
        fifo.clear();
        lock.unlock();

        for (auto& task : ready_task_list) {
            std::cout << "thread task address: " << &task << std::endl;
            task();
        }

        // 判断定时器时间是否到达，如果已经到达则触发定时器
        auto t2 = clock::now();
        auto diff = t2 - last_timeout;
        if (status == std::cv_status::timeout ||
            // diff > clock::duration(std::chrono::milliseconds(400))) {
            diff > std::chrono::milliseconds(400)) {
            last_timeout = t2;

            auto timeout_time = std::chrono::system_clock::now();
            std::cout << "timeout: time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeout_time.time_since_epoch()).count()
                      << std::endl;
        }
    }
    std::cout << "thread stop..." << std::endl;
}

void process_wait_until(std::list<std::function<void()>>& fifo)
{
    // using clock = std::chrono::high_resolution_clock;
    using clock = std::chrono::steady_clock;
    auto last_timeout = clock::now();

    while (!stop) {
        std::cv_status status;
        std::unique_lock<std::mutex> lock(mtx);
        auto t1 = clock::now();
        while (fifo.empty()) {
            // bool ret = cond.wait_until(lock, std::chrono::steady_clock::now() + std::chrono::seconds(1),
            //                            [&fifo] () { return !fifo.empty(); });
            // ret为false则代表超时或者已经过了超时时间返回pred(),用这个分支则不用while去判断fifo.empty()
            status = cond.wait_until(lock, t1 + std::chrono::milliseconds(100));
            if (status == std::cv_status::timeout) {
                break;
            }
        }

        if (status != std::cv_status::timeout) {
            // for (auto it = fifo.begin(); it != fifo.end(); ) {
            //     auto& task = *it;
            //     task();
            //     fifo.erase(it++);
            // }

            auto input_time = std::chrono::system_clock::now();
            std::cout << "handle input: time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(input_time.time_since_epoch()).count()
                      << std::endl;
        }

        std::list<std::function<void()>> ready_task_list;
        ready_task_list = fifo;
        fifo.clear();
        lock.unlock();

        for (auto& task : ready_task_list) {
            std::cout << "thread task address: " << &task << std::endl;
            task();
        }

        // 判断定时器时间是否到达，如果已经到达则触发定时器
        auto t2 = clock::now();
        if (status == std::cv_status::timeout || t2 > last_timeout + std::chrono::milliseconds(100)) {
            last_timeout = t2;

            auto timeout_time = std::chrono::system_clock::now();
            std::cout << "timeout: time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(timeout_time.time_since_epoch()).count()
                      << std::endl;
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
    std::cout << "test" << std::endl;
}

struct data {
    int a = 10;
    int b = 20;
};

int main()
{
    // std::thread t(process, std::ref(fifo));
    std::thread t(process_wait_for, std::ref(fifo));
    // std::thread t(process_wait_until, std::ref(fifo));

    data da;
    std::cout << "main data address: " << &da << std::endl;
    std::function<void()> task = std::bind([](data& da) {
                                               std::ostringstream oss;
                                               oss << "------------------" << std::endl;
                                               oss << "thread data address: " << &da << std::endl;
                                               oss << da.a << std::endl;
                                               oss << da.b << std::endl;
                                               oss << "------------------" << std::endl;
                                               std::cout << oss.str() << std::endl;
                                           }, da); //bind 函数拷贝data
    std::cout << "main task address: " << &task << std::endl;

    // for (int i = 0; i < 30; ++i)
    while (true)
    {
        {
            std::lock_guard<std::mutex> cond_lock(mtx);
            // bool notify = fifo.empty();
            fifo.push_back(task); //push bind function
            fifo.push_back(std::bind([](int x) {
                                         std::cout << x << std::endl;
                                     }, 5555)); //push bind function
            fifo.push_back(test); //push c function
            // if (notify) { //send condition variable when fifo empty
            //     cond.notify_all();
            // } 

            // 这里不用判空来notify，如果内部没有wait好像不会进入系统api
            cond.notify_all();
        }

        auto t1 = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto t2 = std::chrono::steady_clock::now();
        auto diff = t2 - t1;
        // std::cout << "test: diff time: "
        //           << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
        //           << std::endl;
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
