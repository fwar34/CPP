#pragma once

template<typename T>
class Singleton
{
public:
    template<typename... Args>
    static T& GetInstance(Args&&... args)
    {
        static T instance(std::forward<Args>(args)...);
        return instance;
    }

protected:
	Singleton() = default;
	~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};