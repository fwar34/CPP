/*************************************************************************
    > File Name: future_test7.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 PM02:13:41 CST
    > Content: http://www.cnblogs.com/haippy/p/3280643.html
 ************************************************************************/

#include <iostream>
#include <chrono>
#include <future>
#include <thread>

void print(char c, int ms)
{
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        std::cout << c;
    }
}

int main()
{
    std::cout << "with launch::async:\n";
    std::future<void> foo = std::async(std::launch::async, print, '*', 100);
    std::future<void> bar = std::async(std::launch::async, print, '@', 200);
    // async "get" (wait for foo and bar to be ready):
    foo.get();
    bar.get();
    std::cout << "\n\n";

    std::cout << "with launch deferred:\n";
    foo = std::async(std::launch::deferred, print, '*', 100);
    bar = std::async(std::launch::deferred, print, '@', 200);
    // deferred "get" (perform the actual calls):
    foo.get();
    bar.get();
    std::cout << "\n\n";

    return 0;
}
