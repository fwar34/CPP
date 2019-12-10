/*************************************************************************
    > File Name: shared_ptr_test3.cpp
    > Author: Feng
    > Created Time: 2019-12-09 20:01
    > Content: shared_ptr<void> 可以正确的调用对象的析构函数（不论是否是模板类）
 ************************************************************************/

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class Test
{
public:
    Test() { x = 3; }
    ~Test() { cout << "~Test" << endl; }
private:
    int x;
};

template <class T>
class TestT
{
public:
    TestT(T a) { x = a; }
    ~TestT() { cout << "~TestT" << endl; }
private:
    T x;
};

struct Data
{
    shared_ptr<void> data_p;
};

int main()
{
    vector<Data*> data_v;
    Test* p = new Test;
    shared_ptr<void> p1(p);
    for (int i = 0; i < 3; ++i) {
        Data* data = new Data;
        data->data_p = p1;
        cout << data->data_p.use_count() << endl;
        data_v.push_back(data);
    }

    for (auto data : data_v) {
        delete data;
    }

    data_v.clear();
    auto* pt = new TestT<int>(5);
    shared_ptr<void> p1t(pt);
    for (int i = 0; i < 5; ++i) {
        Data* data = new Data;
        data->data_p = p1t;
        cout << data->data_p.use_count() << endl;
        data_v.push_back(data);
    }

    for (auto data : data_v) {
        delete data;
    }

    return 0;
}
