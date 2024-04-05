#include "User.h"
#include "Session.h"
#include "Message.h"

namespace Nt
{

void User::ProcessMessage(std::shared_ptr<Message>& message)
{
    std::cout << "User: " << id_ << " process msg[" << message << "]" << std::endl;
    std::shared_ptr<Message> response;
    session_->SendMessage(response);
}

}