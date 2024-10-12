/*************************************************************************
    > File Name: thread_local.cpp
    > Author: Feng
    > Created Time: 2018/12/18 11:36:45
    > Content: 
 ************************************************************************/

#include <thread>
#include <iostream>

using namespace std;

static int y = 0;
thread_local int z = 0;
auto f = [&]() {
    ++y; ++z;
    cout << y << "," << z << endl;
};

int main()
{
    thread t1(f);
    thread t2(f);
    t1.join();
    t2.join();
    cout << y << "," << z << endl;
    cout << __cplusplus << endl;
    return 0;
}
