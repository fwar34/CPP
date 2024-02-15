#include "ThreadPool.h"
#include <iostream>
#include <chrono>

int main(int argc, char const *argv[])
{
    std::future<int> ret = ThreadPool::Instance().Commit([] (int x) -> int {
        std::cout << "thread tid = " << std::this_thread::get_id() << std::endl;
        return x++;
    }, 334);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "commit return ret = " << ret.get() << std::endl;

    return 0;
}
