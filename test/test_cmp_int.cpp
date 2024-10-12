/*************************************************************************
    > File Name: test_cmp_int.cpp
    > Author: Feng
    > Created Time: Wed 28 Nov 2018 04:11:45 PM CST
    > Content: 
 ************************************************************************/

#include <unistd.h>
#include <time.h>
#include <iostream>

using namespace std;

int main()
{
    time_t tm1;
    time_t tm2;
    time(&tm1);
    sleep(2);
    time(&tm2);
    int num = 1;
    cout << tm1 - tm2 << endl;
    cout << ((tm1 - tm2) < num) << endl;

    int a = 3;
    int b = 4;
    cout << a - b << endl;

    uint32_t c = 3;
    uint32_t d = 4;
    cout << c - d << endl;
    cout << (c - d < num) << endl; //warning compare int and uint32_t, result is error

    return 0;
}
