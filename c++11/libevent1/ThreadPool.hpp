#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <vector>
#include <cstdint>
#include <functional>
#include <future>

class ThreadPool
{
    using Task = std::packaged_task<void()>;
    std::queue<Task> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_;
    std::vector<std::thread> threads_;
    uint32_t threadNum_;

public:
    explicit ThreadPool(int threadNum = std::thread::hardware_concurrency()) : stop_(false), threadNum_(threadNum)
    {
        if (threadNum < 2) {
            threadNum_ = 2;
        }
    }
    ~ThreadPool()
    {
    }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    static ThreadPool& GetInstance()
    {
        static ThreadPool instance;
        return instance;
    }

    template<typename F, typename... Args>
    auto Commit(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_) {
            return std::future<RetType>();
        }

        std::shared_ptr<std::packaged_task<RetType()>> task = 
            std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            std::lock_guard<std::mutex> lk(mutex_);
            tasks_.emplace([task]() { return (*task)(); });
        }
        cv_.notify_one();

        return task->get_future();
    }

    void Start()
    {
        for (uint32_t i = 0; i < threadNum_; ++i) {
            threads_.emplace_back([this]() {
                for (;;) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lk(mutex_);
                        cv_.wait(lk, [this]() { return stop_ || !tasks_.empty(); });
                        if (stop_) {
                            std::cout << "Thread exit..." << std::endl;
                            break;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
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
};