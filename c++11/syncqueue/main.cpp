#include "SyncQueue.h"
#include <iostream>
#include <thread>

int main(int argc, char const *argv[])
{
    SyncQueue<int> que;
    std::thread producer([](SyncQueue<int>* q) {
        int count = 0;
        for (;;) {
            std::cout << "producer: " << count << std::endl;
            q->Push(count++);
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            if (count % 1000== 0) {
                q->Stop();
                std::cout << "thread producer quit" << std::endl;
                break;
            }
        }
    }, &que);

    std::thread consumer([](SyncQueue<int>* q) {
        int count = 0;
        for (;;) {
            if (q->Pop(count)) {
                std::cout << "consumer: " << count << std::endl;
            } else {
                std::cout << "thread consumer quit" << std::endl;
                break;
            }
        }
    }, &que);

    producer.join();
    consumer.join();

    return 0;
}
