#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>
#include <memory>

template<typename T>
class Singleton
{
public:
    template<typename... Args>
    static T& GetInstance(Args&&... args)
    {
        std::call_once(callFlag_, [&args...] {
            // std::make_unique 要调用 T 的构造函数，但是 T 的构造函数声明的是 private，
            // 要么就把 std::make_unique 声明为 T 的友元函数，但是这样就很麻烦，所以直接用 new T 方便
            // instance_.reset(std::make_unique<T>(std::forward<Args>(args)...));
            instance_.reset(new T(std::forward(args)...));
        });
        return *instance_;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static std::once_flag callFlag_;
    static std::unique_ptr<T> instance_;
};

template<typename T>
std::once_flag Singleton<T>::callFlag_;

template<typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

#endif // SINGLETON_H
