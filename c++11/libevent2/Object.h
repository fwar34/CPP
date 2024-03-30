#pragma once
#include <atomic>

class IOThread;
class RefCount
{
public:
    RefCount() : refCount_(1)
    {

    }
    virtual ~RefCount() = default;
    void AddRef()
    {
        refCount_++;
    }

    void ReleaseRef()
    {
        if (--refCount_ == 0) {
            delete this;
        }
    }

    uint32_t UseCount()
    {
        return refCount_.load();
    }

private:
    std::atomic_uint32_t refCount_;
};

class Signal;
class Object : public RefCount
{
public:
    Object(IOthread* thd) : thread_(thd)
    {
    }
    virtual ~Object() = default;
    void SendSignal(const Signal& s);
    void SendStart();
    void SendStop();

protected():
    virtual void ProcessSignal()top();
    {
    }

private:
    IOThread* thread_;
};