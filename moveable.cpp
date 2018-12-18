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
    moveable() {}
    moveable(moveable&& other)
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
    moveable m2(std::move(m1));
    cout << m1.getx() << endl;
    cout << m2.getx() << endl;
    moveable m3 = moveable::create();
    cout << m2.getx() << endl;
    return 0;
}
