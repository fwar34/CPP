#include <inttypes.h>
#include <iostream>
using namespace std;

void print(uint32_t& i)
{
    cout << "lprint " << i << endl;
}

void print(uint32_t&& i)
{
    cout << "rprint " << i << endl;
}

void Forward(uint32_t&& i)
{
    print(i);
}

int main()
{
    uint32_t i = 0;
    //i是左值，调用的是左值参数的print
    print(i);
    //4右值，调用的是右值参数的print
    print(4);
    //5是右值，但是在Forward函数中传递给print函数的时候变成了左值
    //因为在Forward调用print的时候右值5变成了一个命名对象，编译器将其视作左值处理
    //编译器将已命名的右值引用视为左值，将未命名的右值引用视为右值
    Forward(5);
    
    return 0;
}