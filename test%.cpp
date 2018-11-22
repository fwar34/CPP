/*************************************************************************
    > File Name: test.cpp
    > Author: Feng
    > Created Time: Wed 21 Nov 2018 05:54:54 PM CST
    > Content: 
 ************************************************************************/

#include <iostream>

using namespace std;

int main()
{
    unsigned int count = 0;
    while (true) {
        if (count % 30 == 0) {
            cout << count << endl;
        }
        count += 5;
    }
}
