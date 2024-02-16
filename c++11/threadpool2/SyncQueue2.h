#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <iostream>

template<class T>
class SyncQueue2
{
public:
    SyncQueue2(uint32_t maxSize) : stop_(false), maxSize_(maxSize) {}

    template<class Args>
    bool Push(Args&& args)
    // bool Push(T&& t)
    {
        std::cout << "T&& t" << std::endl;
        // return Add(std::forward<T>(t));
        return Add(std::forward<Args>(args));
    }

    // bool Push(const T& t)
    // {
    //     std::cout << "const T& t" << std::endl;
    //     return Add(t);
    // }

    bool Pop(T& t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !list_.empty() || stop_; });
        if (stop_) {
            return false;
        }
        t = std::move(list_.front());
        list_.pop_front();
        if (list_.size() == maxSize_ - 1) {
            notFull_.notify_one();
        }

        return true;
    }

    bool Pop(std::list<T>& l)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !list_.empty() || stop_; });
        if (stop_) {
            return false;
        }
        l = std::move(list_);
        notFull_.notify_one();

        return true;
    }

    void Stop()
    {
        stop_ = true;
        notFull_.notify_all();
        notEmpty_.notify_all();
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return Count();
    }

    uint32_t Count()
    {
        return list_.size();
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return list_.empty();
    }

    bool  Full()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return list_.size() == maxSize_;
    }

private:
    template<class Args>
    bool Add(Args&& args)
    // bool Add(T&& t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return list_.size() != maxSize_ || stop_; });
        if (stop_) {
            return false;
        }
        list_.emplace_back(std::forward<Args>(args));
        // list_.emplace_back(std::forward<T>(t));
        if (list_.size() == 1) {
            notEmpty_.notify_one();
        }
        return true;
    }

private:
    std::list<T> list_;
    std::mutex mutex_;
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
    std::atomic_bool stop_;
    uint32_t maxSize_;
};