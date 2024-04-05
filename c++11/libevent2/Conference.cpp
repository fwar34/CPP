#include "Conference.h"
#include <thread>

namespace Nt
{

void Conference::AddUser(std::shared_ptr<User> &user)
{

}

void Conference::DelUser(uint32_t userId)
{

}

void Conference::DispatchCommand(std::shared_ptr<Message> message)
{
    MessageHeader& header = message->header_;
    switch (header.msgId_)
    {
    case 100:
        std::cout << "Logic thread: " << std::this_thread::get_id() 
            << " Process message: " << " body[" << message << "]" << std::endl;
        break;
    
    default:
        break;
    }
}
    
} // namespace Nt