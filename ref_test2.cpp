#include <inttypes.h>
#include <iostream>
using namespace std;

class A
{
private:
    uint32_t* m_ptr;
public:
    A() : m_ptr(new uint32_t(0))
    {
        cout << "construct" << endl;
    }
    
    A(const A& a) : m_ptr(new uint32_t(*a.m_ptr))
    {
        cout << "copy construct" << endl;
    }
    
    A(A&& a) : m_ptr(a.m_ptr)
    {
        a.m_ptr = nullptr;
        cout << "move construct" << endl;
    }
    
    ~A() 
    {
        delete m_ptr;    
        cout << "destruct" << endl;
    }
};

A Get(bool flag)
{
    A a;
    A b;
    if (flag)
    {
        return a;
    }
    else
    {
        return b;
    }
}

int main()
{
    A a = Get(false);
    
    return 0;
}