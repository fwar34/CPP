#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue(uint32_t maxQueueSize) : maxQueueSize_(maxQueueSize)
    {
    }

    void Push(T t)
    {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(t)));
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.emplace(std::move(data));
        cv_.notify_one();
    }

    bool TryPop(T& t)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }

        t = std::move(*queue_.front());
        queue_.pop();
        return true;
    }

    std::shared_ptr<T> TryPop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::make_shared<T>();
        }
        std::shared_ptr<T> res = queue_.front();
        queue_.pop();
        return res;
    }

    void Pop(T& t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        t = std::move(*queue_.front());
        queue_.pop();
    }

    std::shared_ptr<T> Pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        std::shared_ptr<T> res = queue_.front();
        queue_.pop();
        return res;
    }

private:
    ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;
    
private:
    std::queue<std::shared_ptr<T>> queue_;
    uint32_t maxQueueSize_;
    std::mutex mutex_;
    std::condition_variable cv_;
};