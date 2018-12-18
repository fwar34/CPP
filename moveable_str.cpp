/*************************************************************************
    > File Name: moveable_str.cpp
    > Author: Feng
    > Created Time: 2018/12/18 9:47:45
    > Content: 
 ************************************************************************/

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

class mystring
{
public:
    mystring() : data_(nullptr), len_(0)
    {
        cout << "mystring()" << endl;
    }

    mystring(const char* str)
    {
        cout << "mystring(const char* str)" << endl;
        len_ = strlen(str);
        CopyData(str);
    }

    mystring(const mystring& other)
    {
        cout << "mystring(const mystring& other)" << endl;
        if (other.data_) {
            len_ = other.len_;
            CopyData(other.data_);
        }
    }

    mystring& operator=(const mystring& other) 
    {
        cout << "mystring& operator=(const mystring& other)" << endl;
        if (this != &other) {
            len_ = other.len_;
            DeleteData();
            CopyData(other.data_);
        }
        return *this;
    }

    mystring(mystring&& other)
    {
        cout << "mystring(mystring&& other)" << endl;
        if (this != &other) {
            len_ = other.len_;
            data_ = other.data_;
            other.data_ = nullptr;
            other.len_ = 0;
        }
    }

    mystring& operator=(mystring&& other)
    {
        cout << "mystring& operator=(mystring&& other)" << endl;
        if (this != &other) {
            len_ = other.len_;
            data_ = other.data_;
            other.len_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    ~mystring()
    {
        cout << "~mystring()" << endl;
        DeleteData();
    }

private:
    void CopyData(const char* str)
    {
        if (str) {
            data_ = new char[len_ + 1];
            memcpy(data_, str, len_);
            data_[len_] = '\0';
        }
    }

    void DeleteData()
    {
        if (data_) {
            delete[] data_;
            data_ = nullptr;
        }
    }

private:
    char* data_;
    size_t len_;
};

mystring func()
{
    mystring str = "hello world";
    return str;
}

int main()
{
    mystring str1 = "c++";
    mystring str2(str1); //拷贝构造函数
    mystring str4(std::move(str1)); //转移构造函数(将str1强转成右值引用了)
    cout << "-------------" << endl;
    mystring str3 = func(); //我理解应该要在转移赋值，但是信息没打印，就是没走转移赋值，还没搞清楚
    cout << "-------------" << endl;

    return 0;
}
