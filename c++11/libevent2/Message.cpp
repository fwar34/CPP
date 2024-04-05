#include "Message.h"

namespace Nt
{

std::ostream& operator<<(std::ostream& os, std::shared_ptr<Message>& message)
{
    os << "message: " << Message().Dump() + " body: ";
    std::ostream_iterator<char> output(os);
    std::copy(message->body_.begin(), message->body_.end(), output);
    return os;
}
    
} // namespace Nt