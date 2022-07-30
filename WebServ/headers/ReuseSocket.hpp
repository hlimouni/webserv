#ifndef __REUSESOCKET_HPP
#define __REUSESOCKET_HPP

#include "Socket.hpp"

class ReuseSocket : public wsv::Socket
{

private:
    static int const _backlog = 1024;
    int _option_value;
    socklen_t _option_len;

public:
    ReuseSocket(int domain, int type, int port);
    ~ReuseSocket();
};

#endif