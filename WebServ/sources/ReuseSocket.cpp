#include "../headers/ReuseSocket.hpp"

ReuseSocket::ReuseSocket(int domain, int type, int port) : wsv::Socket(domain, type, port)
{
    _option_len = sizeof(_option_value);
    if (setsockopt(this->get_fd(), SOL_SOCKET, SO_REUSEADDR, &this->_option_value, _option_len) < 0)
        throw std::runtime_error("Couldn't set REUSEADDR option for socket");
    if (setsockopt(this->get_fd(), SOL_SOCKET, SO_REUSEPORT, &this->_option_value, _option_len) < 0)
        throw std::runtime_error("Couldn't set REUSEADDR option for socket");
}

ReuseSocket::~ReuseSocket()
{
}
