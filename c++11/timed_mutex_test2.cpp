/*************************************************************************
    > File Name: timed_mutex_test2.cpp
    > Author: Feng
    > Created Time: Monday, January 28, 2019 PM02:23:36 CST
    > Content: https://www.cnblogs.com/haippy/p/3237213.html
 ************************************************************************/

#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

using namespace std;

std::timed_mutex g_mtx;
void task(int n)
{
    while (!g_mtx.try_lock_for(chrono::milliseconds(200))) {
        cout << '-' << n;
    }

    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "*" << n << endl;
    g_mtx.unlock();
}

int main()
{
    thread ts[10];
    for (int i = 0; i < 10; ++i) {
        ts[i] = thread(task, i);
    }

    for (auto& t : ts) {
        t.join();
    }

    return 0;
}
