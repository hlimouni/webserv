#include "listeningSocket.hpp"

listeningSocket::listeningSocket(int domain, int type, int port) : ReuseSocket(domain, type, port)
{
    this->bind_name();
    this->listen_to_port(this->_backlog);
}

listeningSocket::~listeningSocket()
{
}
