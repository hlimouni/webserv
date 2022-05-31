#include "listeningSocket.hpp"

listeningSocket::listeningSocket(int domain, int type, int port) : wsv::Socket(domain, type, port)
{
    this->bind_name();
    this->listen_to_port(this->_backlog);
}

listeningSocket::~listeningSocket()
{
}
