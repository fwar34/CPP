#include "ThreadSafeQueue.h"
#include <future>
#include <string>
#include <iostream>

std::string Producer(ThreadSafeQueue<std::string>& queue)
{
    queue.Push("aaa");
    queue.Push("bbb");
    return "Producer OK";
}

void Consumer(ThreadSafeQueue<std::string>& queue, std::promise<std::string> promise)
{
    std::shared_ptr<std::string> s = queue.Pop();
    std::cout << "s = " << *s << ", ref count = " << s.use_count() << std::endl;
    std::string ss;
    if (queue.TryPop(ss)) {
        std::cout << "ss = " << ss << std::endl;
    } else {
        std::cout << "queue empty" << std::endl;
    }

    std::string es;
    if (!queue.TryPop(es)) {
        std::cout << "end queue empty" << std::endl;
    }
    promise.set_value_at_thread_exit("Consumer OK");
}

int main(int argc, char const *argv[])
{
    ThreadSafeQueue<std::string> queue(100);
    std::future<std::string> producerFuture = 
        std::async(std::launch::async, Producer, std::ref(queue));
    std::promise<std::string> promise;
    std::future<std::string> consumerFuture = promise.get_future();
    std::thread dbThread(Consumer, std::ref(queue), std::move(promise));

    std::cout << "producerFuture = " << producerFuture.get() << std::endl;
    std::cout << "consumerFufure = " << consumerFuture.get() << std::endl;
    dbThread.join();

    return 0;
}
