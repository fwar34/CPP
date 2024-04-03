#pragma once
#include <memory>
#include <mutex>

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    static std::unique_ptr<T> instance_;

public:
    static T& GetInstance()
    {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            // 如果直接new T时，构造函数抛出异常，会导致instance_保持为nullptr
            // 这里使用 make_unique 来创建对象，避免编译器报错，提高异常安全性，
            // 但是这里 make_unique 需要调用 T 的构造，而 T 的构造是个私有函数，
            // make_unique 无法调用私有函数，可以封装一个函数，将这个函数声明为 T 的友元函数，
            // 这样就可以调用私有函数，但是这样封装起来比较麻烦，使用也麻烦，所以这里直接使用 new 创建对象
            // instance_.reset(std::make_unique<T>().release());
            instance_.reset(new T);
        });
        return *instance_;
    }
    ~Singleton() = default;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;
