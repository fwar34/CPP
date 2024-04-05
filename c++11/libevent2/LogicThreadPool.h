#pragma once
#include "CommonDefine.h"
#include "ConferenceMgr.h"
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <future>

namespace Nt
{

class LogicThreadPool
{
    using Task = std::packaged_task<void()>;
public:
    ~LogicThreadPool()
    {
    }

    static LogicThreadPool& GetInstance()
    {
        static LogicThreadPool instance(LOGIC_THREAD_NUM);
        return instance;
    }

    void Start()
    {
        for (size_t i = 0; i < threadNum_; ++i) {
            threads_.emplace_back([this, i]() {
                Task task;
                for (;;) {
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this, i]() { return !tasks_[i].empty() || stop_; });
                        if (stop_) {
                            std::deque<Task> remainTasks;
                            // remainTasks = std::move(tasks_[i]);
                            remainTasks.swap(tasks_[i]);
                            lock.unlock();
                            for (auto& remainTask : remainTasks) {
                                remainTask();
                            }
                            break;
                        }
                        // std::cout << "Logic thread: " << std::this_thread::get_id() << 
                        //     " recv notify tasks index = " << i << " queue size = " 
                        //     << tasks_[i].size() << std::endl;
                        task = std::move(tasks_[i].front());
                        tasks_[i].pop_front();
                    }
                    task();
                    // std::cout << "Logic thread: " << std::this_thread::get_id() << " execute task complete!" << std::endl;
                }
            });
        }
    }

    void Stop()
    {
        stop_ = true;
        cv_.notify_all();
        for (auto& t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    template <typename F, typename... Args>
    auto Commit(uint32_t confId, F&& f, Args&&... args) -> 
        std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_) {
            return std::future<RetType>();
        }
        // packaged_task 没有拷贝语义，所以需要用 shared_ptr 来控制声明周期，传递到闭包中来延长声明周期
        auto task = std::make_shared<std::packaged_task<RetType()>>([&] {
            std::forward<F>(f)(std::forward<Args>(args)...);
        });
        std::future<RetType> future = task->get_future();
        {
            uint32_t index = confId % threadNum_;
            std::cout << "confId: " << confId << " threadNum_: " << threadNum_ << " index: " << index << std::endl;
            std::cout << "tasks_ size = " << tasks_.size() << std::endl;
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_[index].emplace_back([task] {
                (*task)();
            });
        }
        // 这里现在的实现有惊群现象
        // 这里现在是唤醒所有等待的线程（因为没法唤醒指定的线程，必须唤醒所有线程去看自己的队列是否有任务），可以将 cv_, tasks_ 和 thread 封装在一起，
        // 哪个任务队列有任务了直接唤醒对应线程的 cv_
        std::cout << "notify all thread" << std::endl;
        cv_.notify_all();

        return future;
    }

private:
    LogicThreadPool(size_t threadNum = std::thread::hardware_concurrency()) : 
        threadNum_(threadNum), tasks_(threadNum_), stop_(false)
    {
    }
    size_t threadNum_;
    // 每个线程处理一个队列，实际上可以将线程、任务队列和条件变量封装在一起，就不会惊群
    std::vector<std::deque<Task>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_;
    std::vector<std::thread> threads_;
};
    
} // namespace Nt