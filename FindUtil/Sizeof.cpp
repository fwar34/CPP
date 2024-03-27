#include <iostream>

void Sizeof()
{
    char str[] = "abcdef";
    std::cout << sizeof str << std::endl; // 7：sizeof 返回内存所占用的实际字节数，str 是数组

    const char* p = "abcdef";
    std::cout << sizeof p << std::endl; // 8：sizeof 返回内存所占用的实际字节数，p 是指针

    int array[] = {1, 3, 5};
    std::cout << sizeof array << std::endl; // 12：sizeof 返回内存所占用的实际字节数


}

int main()
{
    Sizeof();

    return 0;
}