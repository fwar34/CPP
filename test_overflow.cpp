#include <iostream>

void TestUnsigned()
{
    // unsigned short max 65535，所以到65535再加1的时候就会溢出，i又成了0，导致循环不会按照要求推出，无限循环
    for (unsigned short i = 0; i <= 65535; i++) {
        std::cout << i << std::endl;
        if (i == 65535) {
            std::cout << "i == 65535" << std::endl;
            getchar();
        }
    }
}

void TestSigned()
{
    // signed short max 32767，32767再加1就成了-32768（因为1000000000000000b为-32768，参考
    // https://www.hello-algo.com/chapter_data_structure/number_encoding/#332)，最终也是一个无限循环
    for (short i = 0; i <= 32767; i++) {
        std::cout << i << std::endl;
        if (i == 32767) {
            std::cout << "i == 32767" << std::endl;
            // std::cin.get();
        }
    }
}

int main(int argc, char const *argv[])
{
    // TestUnsigned();
    TestSigned();
    return 0;
}
