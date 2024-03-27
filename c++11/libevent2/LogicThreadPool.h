#pragma once
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

class LogicThreadPool
{
    using Task = std::packaged_task<void()>;
public:
    ~LogicThreadPool()
    {
    }

    static LogicThreadPool& GetInstance()
    {
        static LogicThreadPool instance;
        return instance;
    }

    bool Start()
    {
        for (size_t i = 0; i < threadNum_; ++i) {
            threads_.emplace_back([this, i]() {
                Task task;
                for (;;) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this, i]() { return !tasks_[i].empty() || stop_; });
                    if (stop_) {
                        std::deque<Task> remainTasks;
                        lock.lock();
                        // remainTasks = std::move(tasks_[i]);
                        remainTasks.swap(tasks_[i]);
                        lock.unlock();
                        for (auto& remainTask : remainTasks) {
                            remainTask();
                        }
                        break;
                    }
                    task = std::move(tasks_[i].front());
                    tasks_[i].pop_front();
                }
                task();
            });
        }
    }

    void Stop()
    {
        stop_ = true;
        cv_.notify_all();
        for (auto& t : threads_) {
            t.join();
        }
    }

    template <typename F, typename... Args>
    auto Commit(F&& f, Args&&... args, uint32_t confId) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_) {
            return std::future<RetType()>();
        }
        auto task = std::make_shared<std::packaged_task<RetType>>([&] {
            std::forward<F>(f)(std::forward<Args>(args)...);
        });
        std::future<RetType> future = task.get_future();
        {
            uint32_t index = confId % threadNum_;
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_[index].emplace_back([task] {
                (*task)();
            });
        }
        // 这里现在是唤醒所有等待的线程（因为没法唤醒指定的线程，必须唤醒所有线程去看自己的队列是否有任务），可以将 cv_, tasks_ 和 thread 封装在一起，
        // 哪个任务队列有任务了直接唤醒对应线程的 cv_
        cv_.notify_all();

        return future;
    }

private:
    LogicThreadPool(size_t threadNum = std::thread::hardware_concurrency()) : threadNum_(threadNum), stop_(false)
    {
    }
    size_t threadNum_;
    std::deque<Task> tasks_[threadNum_]; // 每个线程处理一个队列，实际上可以将线程、任务队列和条件变量封装在一起
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_;
    std::vector<std::thread> threads_;
};