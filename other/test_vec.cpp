/*************************************************************************
    > File Name: test_vec.cpp
    > Author: Feng
    > Created Time: Wed 09 May 2018 04:52:48 PM CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int> a;
    vector<int> b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    vector<int> test = vector<int>();
    cout << test.size() << endl;

    cout << (a == b) << endl;
    return 0;
}
