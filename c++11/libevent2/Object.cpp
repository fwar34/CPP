#include "Object.h"
#include "IOThread.h"
#include "Signal.h"
#include <iostream>

namespace Nt
{
Object::~Object()
{
    if (thread_) {
        thread_->ReleaseRef();
        thread_ = nullptr;
    }
}

void Object::SendSignal(const Signal& s)
{
    AddRef();
    thread_->SendSignal(s);
}

void Object::SendStart()
{
    AddRef();
    Signal signal = { Signal::SIGNAL_START, this };
    thread_->SendSignal(signal);
}
void Object::SendStop()
{
    AddRef();
    Signal signal = { Signal::SIGNAL_STOP, this };
    thread_->SendSignal(signal);
}

void Object::ProcessSignal(Signal::SignalId id)
{
    std::cout << "object: " << this << " receive signal: " << id << std::endl;
}

}