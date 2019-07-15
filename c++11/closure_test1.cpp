/*************************************************************************
    > File Name: closure_test1.cpp
    > Author: Feng
    > Created Time: 2019-07-12 17:25
    > Content: 
************************************************************************/

#include <iostream>
#include <functional>
#include <thread>

void ThreadFunc(void* user_data) {
    auto func = (std::function<void()>*)user_data;
    (*func)();
    delete func;
}

void Test(std::function<void()> callback) {
    auto user_data = new std::function<void()>(callback);
    std::thread t(ThreadFunc, user_data);
    t.detach();
}

int main() {
    int a = 3;
    Test([=]() {
             std::this_thread::sleep_for(std::chrono::seconds(1));
             std::cout << "first " << a << std::endl;
         });
    a = 4;
    Test([=]() {
             std::this_thread::sleep_for(std::chrono::seconds(1));
             std::cout << "second " << a << std::endl;
         });

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
