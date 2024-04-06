#include "Message.h"
#include "iostream"

namespace Nt
{

std::ostream& operator<<(std::ostream& os, std::shared_ptr<Message>& message)
{
    os << "message: " << message->Dump() + " body: ";
    std::ostream_iterator<char> output(os);
    std::copy(message->body_.begin(), message->body_.end(), output);
    return os;
}
    
std::ostream& operator<<(std::ostream& os, Message& message)
{
    os << "message: " << message.Dump() + " body: ";
    std::ostream_iterator<char> output(os);
    std::copy(message.body_.begin(), message.body_.end(), output);
    return os;
}

std::string PrintMessage(const std::shared_ptr<Message>& message)
{
    std::stringstream ss;
    ss << "msgId: " << message->header_.msgId_
        << " bodyLength: " << message->header_.bodyLength_
        << " confId: " << message->header_.confId_ << " body:";
    
    // for (auto c : message->body_) {
    //     ss << c;
    // }
    // for (auto c : message->body_) {
    //     std::cout << "c: " << c << std::endl;
    // }
    ss << message->body_.data();
    ss << "\n";
    return ss.str();
}

void PrintByteArray(const std::vector<char>& body)
{
    std::cout << "ByteArray: ";
    for (char c : body) {
        std::cout << std::hex << static_cast<int>(c) << " ";
    }
    std::cout << "\n";
}
// void CSession::PrintRecvData(char* data, int length) {
// 	stringstream ss;
// 	string result = "0x";
// 	for (int i = 0; i < length; i++) {
// 		string hexstr;
// 		ss << hex << std::setw(2) << std::setfill('0') << int(data[i]) << endl;
// 		ss >> hexstr;
// 		result += hexstr;
// 	}
// 	std::cout << "receive raw data is : " << result << endl;;
// }

} // namespace Nt