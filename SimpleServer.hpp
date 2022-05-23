#include "Socket.hpp"
#include <unistd.h>

namespace WSV
{
    class SimpleServer
    {

    private:
        Socket _sockListen;
        int _sockexchangeFd;
        int addrlen;

    public:
        SimpleServer(Socket const &sock);
        ~SimpleServer();

        void listen_for_reqs(int backlog);
        void accept_req();
        void print_read_msg();
        void write_msg(const char *msg);

        int get_fd() const;
    };
}