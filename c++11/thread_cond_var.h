#ifndef _TANG_CDTS_THREAD_H_
#define _TANG_CDTS_THREAD_H_

#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>

class CdtsThread
{

public:
    bool Start();
    void PushMessage();

private:
    void ThreadLoop();
    void ProcessTask(Task& task);
    
private:
    std::list<FifoData> fifo_;
    std::mutex fifo_mutex_;
    std::condition_variable fifo_cond_;
    std::atomic<bool> thread_stop_;
    std::thread* thread_;
};

#endif
