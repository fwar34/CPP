#include "Conference.h"
#include "User.h"
#include "Session.h"
#include <thread>

namespace Nt
{

void Conference::AddUser(std::shared_ptr<User> &user)
{

}

void Conference::DelUser(uint32_t userId)
{

}

void Conference::DispatchCommand(std::shared_ptr<Message> message, Session* session)
{
    MessageHeader& header = message->header_;
    switch (header.msgId_)
    {
    case 100:
        {
            std::cout << "Logic thread: " << std::this_thread::get_id() 
                << " Process message[" << message << "]" << std::endl;
            // std::cout << "XXXX body size: " << message->body_.size();
            // std::cout << "YYYY body:";
            // for (auto c : message->body_) {
            //     std::cout << c;
            // }
            std::cout << std::endl;
            std::shared_ptr<User> user;
            auto it = users_.find(200);
            if (it == users_.end()) {
                user.reset(new User(200, session));
                users_[200] = user;
            } else {
                user = it->second;
            }
            user->ProcessMessage(message);
            break;
        }
    default:
        break;
    }
}
    
} // namespace Nt