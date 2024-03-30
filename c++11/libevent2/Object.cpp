#include "Object.h"
#include "Signal.h"

void Object::SendSignal(const Signal& s)
{
    AddRef();
    thread_->SendSignal(s);
}

void Object::SendStart()
{
    AddRef();
    NewNt::Signal signal = { NewNt::Signal::SIGNAL_START, this };
    thread_->SendSignal(signal);
}
void Object::SendStop();
{
    AddRef();
    NewNt::Signal signal = { NewNt::Signal::SIGNAL_STOP, this };
    thread_->SendSignal(signal);
}

void Object::ProcessSignal()
{
}