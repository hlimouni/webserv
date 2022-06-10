#ifndef __LISTENINGSOCKET_HPP
#define __LISTENINGSOCKET_HPP

#include "Socket.hpp"
#include "ReuseSocket.hpp"

class listeningSocket : public ReuseSocket
{

private:
    static int const _backlog = 1024;

public:
    listeningSocket(int domain, int type, int port);
    ~listeningSocket();
};

#endif