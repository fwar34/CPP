#ifndef _ASYNCLOG
#define _ASYNCLOG

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <any>
#include <sstream>
#include <cstdint>
#include <vector>
#include <atomic>
#include <iostream>

enum LogLevel : uint8_t
{
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_ERR = 2,
    LOG_LEVEL_DEBUG = 4
};

class ILogTask
{
public:
    ILogTask(LogLevel level) : level_(level) {}
    virtual ~ILogTask() {}
    virtual void Process() = 0;

    LogLevel level_;
    std::vector<std::any> datas_;
};

class LocalLogTask : public ILogTask
{
public:
    LocalLogTask(LogLevel level) : ILogTask(level) {}
    void Process() override
    {
        // for (auto& data : datas_) {
        //     std::cout << data.type() << std::endl;
        // }
    }
};

class NetLogTask : public ILogTask
{
public:
    NetLogTask(LogLevel level) : ILogTask(level) {}
    void Process() override
    {
        // 发送日志到日志服务器
    }
};

class AsyncLog
{
public:
    AsyncLog() : stop_(false)
    {
        thread_ = std::thread([this] {
            std::shared_ptr<ILogTask> task = std::make_shared<ILogTask>();
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return !tasks_.empty() || stop_.load(); });
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task->Process();
        });
    }
    ~AsyncLog() {}
    void Stop()
    {
        stop_.store(true);
        cv_.notify_all();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    template<class... Args>
    void Write(LogLevel level, Args&&... args)
    {
        std::shared_ptr<ILogTask> task = std::make_shared<ILogTask>(new LocalLogTask(level));
        {
            std::lock_guard<std::mutex> lock(mutex_);
            Enqueue(task, args...);
            tasks_.emplace(task);
            if (tasks_.size() == 1) {
                cv_.notify_one();
            }
        }
    }

    static AsyncLog& Instance()
    {
        static AsyncLog instance;
        return instance;
    }

private:
    AsyncLog(const AsyncLog& other) = delete;
    AsyncLog& operator=(const AsyncLog& other) = delete;

    template<class Arg, class... Args>
    void Enqueue(std::shared_ptr<ILogTask>& task, Arg&& arg, Args&&... args)
    {
        task->datas_.emplace_back(std::any(arg));
        Enqueue(task, std::forward<Args>(args)...);
    }

    template<class Arg>
    void Enqueue(std::shared_ptr<ILogTask>& task, Arg&& arg)
    {
        task->datas_.emplace_back(std::any(arg));
    }

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::shared_ptr<ILogTask>> tasks_; 
    std::ostringstream oss_;
    std::atomic_bool stop_;
};

#endif