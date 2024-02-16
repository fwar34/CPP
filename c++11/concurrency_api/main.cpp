#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

int ReadDb(const std::string& url)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "read db from " << url << " complete" << std::endl;
    return 333;
}

void TestAsync()
{
    std::cout << "main thread read db" << std::endl;
    std::future<int> ret = std::async(std::launch::async, ReadDb, "127.0.0.1");
    // std::future<int> ret = std::async(ReadDb, "127.0.0.1");
    std::cout << "main thread continue doing other" << std::endl;
    std::cout << ret.get() << std::endl;
}

// void ReadRedis(const std::string& url, std::promise<std::string>& promise)
void ReadRedis(const std::string& url, std::promise<std::string>&& promise)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "read redis from " << url << " complete!" << std::endl;
    promise.set_value("hhahah");
}

void WaitRedisResult(std::shared_future<std::string> future)
{
    try {
        std::cout << "thread " << std::this_thread::get_id() << " get redis result = " << future.get() << std::endl;
    } catch(const std::future_error& e) {
        std::cerr << e.what() << '\n';
    }
}

void TestPromiseAndSharedFuture()
{
    std::promise<std::string> promise;
    std::shared_future<std::string> future = promise.get_future();
    // auto ret = std::async(std::launch::async, ReadRedis, "localhost", std::move(promise));
    std::thread redis(ReadRedis, "localhost", std::move(promise));
    // std::thread redis(ReadRedis, "localhost", std::ref(promise));
    std::thread t1(WaitRedisResult, future);
    std::thread t2(WaitRedisResult, future);

    redis.join();
    t1.join();
    t2.join();
}

int main(int argc, char const *argv[])
{
    TestAsync();
    TestPromiseAndSharedFuture();

    return 0;
}
