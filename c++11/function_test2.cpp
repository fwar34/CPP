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

std::condition_variable cond;
std::mutex mtx;
bool stop = false;

std::list<std::function<void()>> fifo;

void process(std::list<std::function<void()>>& fifo)
{
    while (!stop) {
        std::unique_lock<std::mutex> lock(mtx);
        while (fifo.empty()) {
            cond.wait(lock);
        }
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

int main()
{
    std::thread t(process, std::ref(fifo));

    std::function<void()> task = std::bind([](int x) {
                                               std::cout << x << std::endl;
                                           }, 44444);
    std::cout << "main task address: " << &task << std::endl;

    {
        std::lock_guard<std::mutex> cond_lock(mtx);
        bool notify = fifo.empty();
        fifo.push_back(task); //push bind function
        fifo.push_back(std::bind([](int x) {
                                     std::cout << x << std::endl;
                                 }, 5555)); //push bind function
        fifo.push_back(test); //push c function
        if (notify) //send condition variable when fifo empty
            cond.notify_all();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //stop thread
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
        fifo.push_back([](){});
        cond.notify_all();
        std::cout << "send stop" << std::endl;
    }

    t.join();

    return 0;
}
