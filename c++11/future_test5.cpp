/*************************************************************************
    > File Name: future_test5.cpp
    > Author: Feng
    > Created Time: Wednesday, January 30, 2019 AM11:30:15 CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <future>

int get_value() { return 10; }

int main()
{
    std::future<int> fut = std::async(get_value);
    std::shared_future<int> shared_fut = fut.share();

    std::cout << "value: " << shared_fut.get() << std::endl;
    std::cout << "its double: " << shared_fut.get() * 2 << std::endl;
    return 0;
}
