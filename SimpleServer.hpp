#include "Socket.hpp"

namespace WSV
{
    class SimpleServer
    {

    private:
        Socket _sockListen;
        int _sockexchangeFd;

    public:
        SimpleServer(Socket const &sock);
        ~SimpleServer();

        void listen_for_reqs(int backlog);
        void accept_req();
    };
}