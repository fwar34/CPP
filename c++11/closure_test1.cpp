/*************************************************************************
    > File Name: closure_test1.cpp
    > Author: Feng
    > Created Time: 2019-07-12 17:25
    > Content: 
 ************************************************************************/

#include <iostream>
#include <functional>

int main() {
    int a = 3;
    std::function<void()> fun1 = [=]() { std::cout << a << std::endl; };
    a = 4;
    std::function<void()> fun2 = [=]() { std::cout << a << std::endl; };
    fun1();
    fun2();

    return 0;
}
