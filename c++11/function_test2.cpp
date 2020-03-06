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

std::condition_variable cond;
std::mutex mtx;
bool stop = false;

std::list<std::function<void()> > fifo;

void process(std::list<std::function<void()> >& fifo)
{
    while (!stop) {
        std::unique_lock<std::mutex> lock(mtx);
        while (fifo.empty()) {
            cond.wait(lock);
        }
        std::list<std::function<void()> > ready_task_list;
        ready_task_list = fifo;
        fifo.clear();
        lock.unlock();

        for (auto& task : ready_task_list) {
            std::cout << "thread task address: " << &task << std::endl;
            task();
        }
    }
}

int main()
{
    std::thread t(process, fifo);

    std::function<void()> task = std::bind([](int x) {
                                               std::cout << x << std::endl;
                                           }, 44444);
    std::cout << "main task address: " << &task << std::endl;

    {
        std::unique_lock<std::mutex> cond_lock(mtx);
        bool notify = fifo.empty();
        fifo.push_back(task);
        if (notify)
            cond.notify_all();
    }

    t.join();

    return 0;
}
