/*************************************************************************
    > File Name: forward.cpp
    > Author: Feng
    > Created Time: 2018/12/18 10:57:04
    > Content: 
 ************************************************************************/

#include <iostream>

using namespace std;

void check(int& lr)
{
    cout << "lvalue" << endl;
}

void check(int&& rr)
{
    cout << "rvalue" << endl;
}

template <typename T>
void print(T&& v)
{
    check(std::forward<T>(v)); //完美转发，一句参数类型调用不同函数
}

int main()
{
    int x = 10;
    print(x);  //传递左值引用，output lvalue
    print(std::move(x)); //传递右值引用，output rvalue
    print(std::forward<int>(x)); //传递右值引用，output rvalue
    return 0;
}
