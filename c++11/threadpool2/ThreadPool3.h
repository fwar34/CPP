#pragma once

#include "SyncQueue2.h"
#include <iostream>
#include <thread>
#include <functional>
#include <vector>

class ThreadPool3
{
    using Task = std::function<void()>;
public:
    ThreadPool3(uint32_t num = std::thread::hardware_concurrency()) : tasks_(MaxTaskCount), threadNum_(num)
    {
        Start();
    }
    ~ThreadPool3()
    {
        Stop();
    }

    static ThreadPool3& Instance()
    {
        static ThreadPool3 instance;
        return instance;
    }

    void Stop()
    {
        std::cout << "ThreadPool3 Stop" << std::endl;
        std::call_once(stopFlag_, [this] { StopThreads(); });
    }

    void PushTask(const Task& task)
    {
        tasks_.Push(task);
    }

private:
    void Start()
    {
        for (uint32_t i = 0; i < threadNum_; ++i) {
            threads_.emplace_back([this] {
                for (;;) {
                    Task task;
                    if (tasks_.Pop(task)) {
                        task();
                    } else {
                        std::cout << "thread end!" << std::endl;
                        break;
                    }
                }
            });
        }
    }

    void StopThreads()
    {
        std::cout << "ThreadPool3 StopThreads" << std::endl;
        tasks_.Stop();
        for (auto& td : threads_) {
            td.join();
        }
    }

private:
    std::vector<std::thread> threads_;
    SyncQueue2<Task> tasks_;
    uint32_t threadNum_;
    std::once_flag stopFlag_;
};