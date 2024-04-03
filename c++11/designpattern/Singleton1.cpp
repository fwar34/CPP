#include <iostream>
#include <memory>
#include <mutex>

template<typename T>
class Singleton
{
protected:
    // 基类的构造声明为 protected 保证派生类可以调用，外部无法调用
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

public:
    static T& GetInstance()
    {
        // 类的静态成员 instance_ 通过 & 捕获列表传入闭包
        static std::once_flag called_flag;
        std::call_once(called_flag, [&] {
            instance_.reset(new T);
        });
        return *instance_;
    }

private:
    static std::unique_ptr<T> instance_;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

class Test : public Singleton<Test>
{
    // 基类 Singleton 的 GetInstance 函数要 new T，
    // 会调用派生类的构造函数，所以必须声明为派生类的友元类
    friend class Singleton<Test>; 
public:
    void Print()
    {
        std::cout << "Test::Print" << std::endl;
    }
private:
    // 派生类的构造声明为 private，保证外部无法调用
    Test() = default;
};

int main()
{
    Test::GetInstance().Print();

    return 0;
}