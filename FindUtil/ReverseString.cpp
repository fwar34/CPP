#include <iostream>
#include <cstring>

// 使用一个数组来存储
void ReverseString(const char* src, char* dst)
{
    size_t len = strlen(src);
    for (size_t i = 0; i < len; i++) {
        dst[len - i - 1] = src[i];
    }
}

// 不适用额外的数组来存储
void ReverseString2(char* str)
{
    std::size_t len = strlen(str);
    for (std::size_t i = 0; i < len / 2; ++i) {
        char tmp = str[len - i - 1];
        str[len -i - 1] = str[i];
        str[i] = tmp;
    }
}

int main()
{
    const char* src = "123456";
    char dst[strlen(src) + 1] = {0};
    ReverseString(src, dst);
    std::cout << dst << std::endl;

    char src2[] = "abcdef";
    ReverseString2(src2);
    std::cout << src2 << std::endl;

    return 0;
}