#include <iostream>
#include <memory>
#include <mutex>

template<typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

public:
    // 可以调用带参数的派生类构造函数
    template<typename... Args>
    static T& GetInstance(Args&&... args)
    {
        std::call_once(call_flag_, [](std::unique_ptr<T>& instance, Args&&... args) {
            instance.reset(new T(std::forward<Args>(args)...));
        }, instance_, std::forward<Args>(args)...);
        return *instance_;
    }

private:
    static std::unique_ptr<T> instance_;
    static std::once_flag call_flag_;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;
template<typename T>
std::once_flag Singleton<T>::call_flag_;

class Test : public Singleton<Test>
{
    friend class Singleton<Test>;
public:
    void Print()
    {
        std::cout << "Test::Print x_ = " << x_ << std::endl;
    }

private:
    Test(int x) : x_(x) {}
    int x_;
};

int main()
{
    Test::GetInstance(3333).Print();

    return 0;
}