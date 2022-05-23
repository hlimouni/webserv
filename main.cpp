#include "SimpleServer.hpp"

#define PORT 8080

int main()
{
    WSV::Socket listenSock(AF_INET, SOCK_STREAM, 8080);

    listenSock.bind_name();
    WSV::SimpleServer sv(listenSock);
    sv.listen_for_reqs(10);

    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection +++++++++++\n\n";
        sv.accept_req();
    }
}