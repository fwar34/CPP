#include "tang_cdts_thread.h"

CdtsThread::CdtsThread()
{
    thread_ = nullptr;
    thread_stop_ = false;
}

CdtsThread::~CdtsThread()
{
    
}

bool CdtsThread::Start()
{
    thread_ = new thread(CdtsThread::ThreadLoop);
    TANG_LOG_INFO("cdts thread start");
}

void CdtsThread::Stop()
{
    {
        std::unique_lock<std::mutex> lock(fifo_mutex_);
        thread_stop_ = true;
        fifo_cond_.notify_one();
    }

    thread_->join();
}

void CdtsThread::PushMessage(FifoData& data)
{
    bool notify = false;
    std::unique_lock<std::mutex> lock(mutex_);
    if (fifo_.empty()) {
        notify = true;
    }
    fifo_.push_back(data);
    if (notify) {
        cond_.notify_one();
    }
}

void CdtsThread::ThreadLoop()
{
    TANG_LOG_INFO("cdts thread begin");
    while (!thread_stop_) {
        std::unique_lock<std::mutex> lock(fifo_mutex_);
        while (fifo_.empty() && !thread_stop_) {
            fifo_cond_.wait(lock);
        }

        while (!fifo_.empty() && !thread_stop_) {
            FifoData& data = fifo_.front();
            lock.unlock();
            ProcessMessage(data);
            lock.lock();
        }
        lock.unlock();
    }

    TANG_LOG_INFO("cdts thread end");
}

void CdtsThread::ProcessTask(FifoData& data)
{
    
}
