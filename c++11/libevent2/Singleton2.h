// 说明：
// 
// 模板参数 T：表示要成为单例的具体类类型，这样任何继承自SingletonBase的类都会自动获得单例特性。
// 
// 保护构造函数与析构函数：将构造函数声明为受保护的，使得只有派生类可以访问，同时阻止外部直接实例化基类。析构函数通常保持为虚函数，以便在多层继承时能够正确地调用派生类的析构函数。
// 
// 删除拷贝构造函数和赋值操作符：为了避免单例对象被复制或赋值，显式声明并禁用这两个操作符。
// 
// 静态方法 getInstance()：这是获取单例实例的唯一公共接口。它使用std::call_once配合std::once_flag确保单例实例只被初始化一次，即使在多线程环境中也能保证线程安全性。
// 
// 静态私有成员：
// 
// instance：存储单例实例的智能指针，使用std::unique_ptr自动管理资源释放，避免内存泄漏。
// initInstanceFlag：配合std::call_once使用的标志，确保初始化逻辑仅执行一次。
// 使用示例：
// 
// 假设有一个类MyClass需要作为单例，只需让它继承SingletonBase<MyClass>：
// 
// cpp
// class MyClass : public SingletonBase<MyClass> {
//    // ... 类的其他实现 ...
// };
#pragma once
#include <mutex>
#include <memory>

// 假设CMutex类为一个简单的互斥锁封装，这里直接使用std::mutex作为示例
template <typename T>
class SingletonBase {
protected:
    SingletonBase() = default; // 私有化默认构造函数，防止外部直接实例化
    ~SingletonBase() = default; // 默认析构函数，通常保持为虚函数以支持派生类的正确销毁

public:
    SingletonBase(const SingletonBase&) = delete; // 删除拷贝构造函数
    SingletonBase& operator=(const SingletonBase&) = delete; // 删除赋值操作符

    static T& getInstance() {
        std::call_once(initInstanceFlag, &SingletonBase<T>::createInstance);
        return *instance;
    }

private:
    static void createInstance() {
        instance = std::unique_ptr<T>(new T());
    }

    static std::unique_ptr<T> instance;
    static std::once_flag initInstanceFlag;
};

// 静态成员变量必须在类外定义
template <typename T>
std::unique_ptr<T> SingletonBase<T>::instance;

template <typename T>
std::once_flag SingletonBase<T>::initInstanceFlag;