/*************************************************************************
    > File Name: atomic_test2.cpp
    > Author: Feng
    > Created Time: Thursday, January 31, 2019 PM01:44:38 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <sstream>
#include <algorithm>

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
std::stringstream stream;

void append_number(int x)
{
    while (lock_stream.test_and_set()) {
    }

    stream << "thread #" << x << std::endl;
    lock_stream.clear();
}

int main()
{
    std::vector<std::thread> ts;
    for (int i = 0; i < 10; ++i) {
        ts.emplace_back(append_number, i + 1);
    }

    std::for_each(ts.begin(), ts.end(), std::mem_fn(&std::thread::join));
    std::cout << stream.str() << std::endl;

    return 0;
}
