#pragma once
#include "Signal.h"
#include <atomic>

namespace Nt
{

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

class IOThread;
class Object : public RefCount
{
public:
    Object(IOThread* thd) : thread_(thd)
    {
    }
    virtual ~Object() = default;
    void SendSignal(const Signal& s);
    void SendStart();
    void SendStop();

protected:
    virtual void ProcessSignal(Signal::SignalId id);

private:
    IOThread* thread_;
};

};