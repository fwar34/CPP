/*************************************************************************
    > File Name: function_test1.cpp
    > Author: Feng
    > Created Time: 2019-07-19 15:56
    > Content: 
 ************************************************************************/

#include <iostream>
#include <functional>

int main()
{
    auto* data = new std::function<void(int)>([](int result) {
        std::cout << result << std::endl;
    });
    (*data)(3);
    return 0;
}
