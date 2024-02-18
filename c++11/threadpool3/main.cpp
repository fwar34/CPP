#include "ThreadPool.h"
#include <iostream>
#include <functional>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

class Point
{
public:
    Point(int x, int y) : x_(x), y_(y) {}
    Point(const Point& other)
    {
        std::cout << "Point copy constructor called!" << std::endl;
        x_ = other.x_;
        y_ = other.y_;
        data_ = other.data_;
    }
    Point& operator=(const Point& other)
    {
        std::cout << "Point assign = called!" << std::endl;
        x_ = other.x_;
        y_ = other.y_;
        data_ = other.data_;
        return *this;
    }
    Point(Point&& other)
    {
        std::cout << "Point move constructor called!" << std::endl;
        x_ = other.x_;
        y_ = other.y_;
        data_.swap(other.data_);
    }

    Point& operator=(Point&& other)
    {
        std::cout << "Point move assign = called!" << std::endl;
        x_ = other.x_;
        y_ = other.y_;
        data_.swap(other.data_);
        return *this;
    }

    int x_;
    int y_;
    std::shared_ptr<int> data_;
};

void Test1()
{
    std::cout << "Test1 start......................................" << std::endl;
    Point point {3, 4};
    auto ret = ThreadPool::Instance().PushTask([point] { // 闭包捕获 point 触发拷贝构造
        std::this_thread::sleep_for(300ms);
        std::cout << "point x = " << point.x_ << " y = " << point.y_ << std::endl;
        return "OK";
    });
    std::cout << ret.get() << std::endl;
}

void Test2()
{
    std::cout << "Test2 start......................................" << std::endl;
    Point point {7, 9};
    auto ret = ThreadPool::Instance().PushTask2(std::bind([point] (int x) { // 闭包捕获 point 触发拷贝构造，闭包右值作为参数调用 bind 再触发一次移动构造
        std::cout << "point x = " << point.x_ << " y = " << point.y_ << std::endl;
        // return Point {x, x};
        return "Test2 OK";
    }, 100));

    // Point p = ret.get();
    // std::cout << "new point " << p.x_ << " " << p.y_ << std::endl;
    std::cout << ret.get() << std::endl;
}

void Test3()
{
    std::cout << "Test3 start......................................" << std::endl;
    Point point {11, 23};
    auto ret = ThreadPool::Instance().PushTask3([] (Point p) {
        std::cout << "point x = " << p.x_ << " y = " << p.y_ << std::endl;
        return std::string("Test3 end ").append("OK");
    }, point);
    // auto ret = ThreadPool::Instance().PushTask3([point] {
    //     std::cout << "point x = " << point.x_ << " y = " << point.y_ << std::endl;
    //     return std::string("Test3 end ").append("OK");
    // });
    std::cout << ret.get() << std::endl;
}
int main(int argc, char const *argv[])
{
    Test1();
    Test2();
    Test3();
    
    return 0;
}
