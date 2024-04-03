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
    static T& GetInstance()
    {
        // 通过参数传递 instance_ 的引用
        std::call_once(call_flag_, [](auto& instance) {
            instance.reset(new T);
        }, instance_);

        // std::call_once(call_flag_, [] (std::unique_ptr<T>& instance) {
        //     instance.reset(new T);
        // }, instance_);

        // std::call_once(call_flag_, [&instance_] {
        //     instance_.reset(new T);
        // });
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
        std::cout << "Test::Print" << std::endl;
    }

private:
    Test() = default;
};

int main()
{
    Test::GetInstance().Print();

    return 0;
}