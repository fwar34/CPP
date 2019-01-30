/*************************************************************************
    > File Name: condition_variable_test2.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 PM02:51:44 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

std::mutex g_mtx;
std::condition_variable g_cv;

int cargo = 0;
bool shipment_available()
{
    return cargo != 0;
}

// 消费者线程
void consume(int n)
{
    for (int i = 0; i < n; ++i) {
        std::unique_lock<std::mutex> lck(g_mtx);
        //在第二种情况下（即设置了 Predicate），只有当 shipment_available 条件为 false 时调用 
        //wait() 才会阻塞当前线程，并且在收到其他线程的通知后只有当 shipment_available 为 true 时才会被解除阻塞。
        g_cv.wait(lck, shipment_available);
        std::cout << cargo << std::endl;
        cargo = 0;
    }
}

int main()
{
    std::thread consumer_thread(consume, 10);
    // 主线程为生产者线程, 生产 10 个物品
    for (int i = 0; i < 10; ++i) {
        while (shipment_available()) {
            std::this_thread::yield();
        }
        std::unique_lock<std::mutex> lck(g_mtx);
        cargo = i + 1;
        g_cv.notify_all();
    }

    consumer_thread.join();
    return 0;
}
