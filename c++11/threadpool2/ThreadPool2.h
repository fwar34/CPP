#pragma once
#include "SyncQueue2.h"
#include <atomic>
#include <vector>
#include <thread>
#include <functional>
#include <future>

static constexpr uint32_t MaxTaskCount = 100;

class ThreadPool2
{
    using Task = std::packaged_task<void()>;
public:
    ThreadPool2(uint32_t threadNum = std::thread::hardware_concurrency()) : threadNum_(threadNum), tasks_(MaxTaskCount) 
    {
        Start();
    }
    ~ThreadPool2()
    {
        Stop();
    }

    template<class F, class... Args>
    auto PushTask(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> ret = task->get_future();
        if (!tasks_.Push([task] { (*task)(); })) {
            return std::future<RetType>{};
        }
        return ret;
    }

    // template<class F, class... Args>
    // auto PushTask2(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    // {
    //     using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
    //     std::packaged_task<RetType()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    //     std::future<RetType> ret = task.get_future();
    //     if (!tasks_.Push([&task] { task(); })) {
    //         return std::future<RetType>{};
    //     }
    //     return ret;
    // }

    void Stop()
    {
        std::call_once(stopFlag_, [this] { StopThreads(); });
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
                        std::cout << "thread id = " << std::this_thread::get_id() << " end!" << std::endl;
                        break;
                    }
                }
            });
        }
    }

    void StopThreads()
    {
        tasks_.Stop();
        for (auto& td : threads_) {
            td.join();
        }
    }

private:
    std::vector<std::thread> threads_;
    uint32_t threadNum_;
    SyncQueue2<Task> tasks_;
    std::once_flag stopFlag_;
};