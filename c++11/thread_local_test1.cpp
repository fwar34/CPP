#include <iostream>
#include <thread>
#include <string>
#include <mutex>
using namespace std;

class Mgr
{
public:
    static thread_local Mgr instance_;
    Mgr()
    {
        cout << "struct " << pthread_self() << ":";
        cout << this << endl;
    }
    static Mgr& instance()
    {
        cout << &instance_ << endl;
        return instance_;
    }
};

thread_local Mgr Mgr::instance_;

void func()
{
    cout << "thread function " << pthread_self() << ":";
    Mgr::instance();
}

int main()
{
    thread t1(func);
    t1.join();
    thread t2(func);
    t2.join();

    return 0;
}
