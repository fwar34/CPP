#ifndef _THREADPOOL
#define _THREADPOOL

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <cstdint>
#include <vector>
#include <queue>
#include <atomic>

class ThreadPool
{
    using Task = std::packaged_task<void()>;
public:
    ThreadPool(uint32_t num = std::thread::hardware_concurrency()) : freeThreadNum_(num), stop_(false)
    {
        if (num < 2) {
            freeThreadNum_ = 2;
        }
        Start();
    }
    ~ThreadPool()
    {
        Stop();
    }
    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    static ThreadPool& Instance()
    {
        static ThreadPool instance;
        return instance;
    }

    template<class F, class... Args>
    auto Commit(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_.load()) {
            return std::future<RetType>{};
        }

        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.emplace([task] {
                (*task)();
            });
        }
        cv_.notify_one();
        return ret;
    }

private:
    void Start()
    {
        for (uint32_t i = 0; i < freeThreadNum_; ++i) {
            threads_.emplace_back([this] {
                while (!stop_.load()) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this] { return !tasks_.empty() || stop_.load(); });
                        if (stop_.load()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    freeThreadNum_--;
                    task();
                    freeThreadNum_++;
                }
            });
        }
    }

    void Stop()
    {
        stop_.store(true);
        cv_.notify_all();
        for (auto& td : threads_) {
            if (td.joinable()) {
                td.join();
            }
        }
    }

private:
    std::vector<std::thread> threads_;
    std::queue<Task> tasks_;
    std::atomic<uint32_t> freeThreadNum_;
    std::atomic<bool> stop_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

#endif
