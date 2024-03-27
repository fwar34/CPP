// 编写一个函数，函数接收一个字符串,是由十六进制数组成的一组字符串,函数的功能是把接到的这组字符串转换成十进制数字.并将十进制数字返回。

#include <iostream>
#include <cstring>

bool ConvertHexStr2Int(const char* hexstr, int& idec)
{
    size_t len = strlen(hexstr);
    if (len > 8) {
        // 长度大于8，大于 INT_MAX 了，返回错误
        return false;
    }

    int num = 0;
    for (size_t i = 0; i < len; ++i) {
        if (hexstr[i] >= '0' && hexstr[i] <= '9') {
            num = hexstr[i] - '0';
        } else if (hexstr[i] >= 'a' && hexstr[i] <= 'f') {
            num = hexstr[i] - 'a' + 10;
        } else if (hexstr[i] >= 'A' && hexstr[i] <= 'F') {
            num = hexstr[i] - 'A' + 10;
        } else {
            return false;
        }
    }
    num *= (2 << (((len - i - 1) * 4) - 1));
    idec += num;

    return true;
}

int main()
{
    int ret = 0;
    if (ConvertHexStr2Int("123A", ret)) {
        std::cout << ret << std::endl;
    } else {
        std::cout << "error!!!" << std::endl;
    }

    return 0;
}