#include "User.h"
#include "Session.h"
#include "Message.h"

void User::ProcessMessage(std::shared_ptr<Message>& message)
{
    std::cout << "User: " << id_ << "process msg[" << message << "]" << std::endl;
    std::shared_ptr<Message> response;
    session_->SendMsg(response);
}