#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <vector>
#include <cstdint>
#include <atomic>
#include <iostream>

class ThreadPool
{
    using Task = std::function<void()>;
    std::queue<Task> tasks_;
    std::vector<std::thread> threads_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::once_flag stopFlag_;
    uint32_t threadCount_;
    std::atomic_bool stop_;
public:
    ThreadPool(uint32_t threadCount = std::thread::hardware_concurrency()) : threadCount_(threadCount), stop_(false)
    {
        Start();
    }
    ~ThreadPool()
    {
        Stop();
    }

    static ThreadPool& Instance()
    {
        static ThreadPool instance;
        return instance;
    }

    template<class F>
    auto PushTask(F func) -> std::future<decltype(func())>
    {
        using RetType = decltype(func());
        // 这里使用智能指针是因为 tasks_ 接收的是 void() 类型，而要传递闭包给 tasks_ 的时候就需要捕获 packaged_task 对象，
        // 而 packaged_task 不支持拷贝构造，所以使用 shared_ptr 来传递
        std::cout << "111" << std::endl;
        // 这里使用 std::move 实际上和 PushTask2(T&& t) 效果一样，这里 std::move 会触发一次移动构造
        std::shared_ptr<std::packaged_task<RetType()>> task = std::make_shared<std::packaged_task<RetType()>>(std::move(func));
        std::cout << "222" << std::endl;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return task->get_future();
    }

    template<class F>
    auto PushTask2(F&& func) -> std::future<decltype(std::forward<F>(func)())>
    {
        using RetType = decltype(std::forward<F>(func)());
        std::cout << "3333" << std::endl;
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::forward<F>(func));
        std::cout << "4444" << std::endl;
        {
            std::lock_guard lk(mtx_);
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return task->get_future();
    }

    template<class F, class...Args>
    auto PushTask3(F&& f, Args&&... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
    {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        std::cout << "5555" << std::endl;
        std::shared_ptr<std::packaged_task<RetType()>> task = 
            std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...)); //
        std::cout << "6666" << std::endl;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return task->get_future();
    }

    void Stop()
    {
        std::call_once(stopFlag_, [this] { StopThreads(); });
    }

private:
    void Start()
    {
        for (uint32_t i = 0; i < threadCount_; ++i) {
            threads_.emplace_back([this] {
                while (!stop_.load()) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lk(mtx_);
                        cv_.wait(lk, [this] { return !tasks_.empty() || stop_.load(); });
                        if (stop_.load()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    void StopThreads()
    {
        stop_.store(true);
        cv_.notify_all();
        for (auto& td : threads_) {
            if (td.joinable()) {
                td.join();
            }
        }
    }
};