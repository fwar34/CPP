#include "Server.h"

int main(int argc, char const *argv[])
{
    return Server::GetInstance().Start();
}
