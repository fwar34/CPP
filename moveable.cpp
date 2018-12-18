/*************************************************************************
    > File Name: moveable.cpp
    > Author: Feng
    > Created Time: 2018/12/18 9:06:04
    > Content: 
 ************************************************************************/

#include <iostream>

using namespace std;

class moveable
{
private:
    int x;
public:
    moveable() { x = 20; }
    moveable(moveable&& other)// : x(-1)
    {
        std::swap(x, other.x);
    }

    moveable& operator=(moveable&& other)
    {
        std::swap(x, other.x);
        return *this;
    }

    int getx() { return x; }

    static moveable create()
    {
        moveable obj;
        return obj;
    }
};

int main()
{
    moveable m1;
    moveable m2(std::move(m1)); //调用转移构造函数,m1被转移到m2，m1内部的x
                                //已经无效(和m2内部的x交换了值，m2内部的x是随机的，
                                //因为在转移构造函数里面没有给m2的x赋初始值)
    cout << m1.getx() << endl; 
    cout << m2.getx() << endl;
    moveable m3 = moveable::create(); //(因为create返回了一个右值)调用转移赋值函数
    cout << m3.getx() << endl;
    return 0;
}
