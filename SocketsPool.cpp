#include "SocketsPool.hpp"

SocketsPool::SocketsPool()
{
    _timeout.tv_sec = 1;
    _timeout.tv_usec = 0;
    FD_ZERO(&this->_fdPool);
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
}

SocketsPool::SocketsPool(listeningSocket const & listenSock)
{
    _timeout.tv_sec = 1;
    _timeout.tv_usec = 0;
    FD_ZERO(&this->_fdPool);
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
    FD_SET(listenSock.get_fd(), &this->_fdPool);
    this->_descriptors.insert(listenSock.get_fd());
}

void SocketsPool::addToRead(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), )
}

SocketsPool::~SocketsPool()
{
}