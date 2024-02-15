#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<class T>
class SyncQueue
{
public:
    void Push(T&& t)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.emplace(std::forward<T>(t));
        if (queue_.size() == 1) {
            cv_.notify_one();
        }
    }

    // 返回 false 的时候代表停止队列
    bool Pop(T& t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {
            return !queue_.empty() || stop_;
        });
        if (stop_) {
            return false;
        }
        t = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void Stop()
    {
        stop_ = true;
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic_bool stop_ = false;
};