#include "SimpleServer.hpp"
#include "WebServer.hpp"

#define PORT 8080
#define HELLO "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"

int main()
{
    wsv::Socket listenSock(AF_INET, SOCK_STREAM, 8080);

    listenSock.bind_name();
    wsv::SimpleServer sv(listenSock);
    sv.listen_for_reqs(10);
    while (1)
    {
        std::cout << "\n+++++++ Waiting for new connection +++++++++++\n\n";
        sv.accept_req();
        
        sv.print_read_msg();
        sv.write_msg(HELLO);
        std::cout << "---------------------------Hello message set-------------------------------\n";
        close(sv.get_fd());
    }


}