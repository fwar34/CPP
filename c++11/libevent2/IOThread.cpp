#include "IOThread.h"

void IOThread::SendSignal(const Signal& s)
{
    signalBox_->Push(s);
}