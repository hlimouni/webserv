#include "SocketsPool.hpp"

SocketsPool::SocketsPool()
{
    _timeout.tv_sec = 1;
    _timeout.tv_usec = 0;
    FD_ZERO(&this->_fdPool);
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
    FD_ZERO(&this->_exepFds);
}

SocketsPool::SocketsPool(listeningSocket const & listenSock)
{
    _timeout.tv_sec = 1;
    _timeout.tv_usec = 0;
    FD_ZERO(&this->_fdPool);
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
    FD_ZERO(&this->_exepFds);
    FD_SET(listenSock.get_fd(), &this->_fdPool);
    this->_descriptors.insert(listenSock.get_fd());
}

void SocketsPool::addToRead(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), &this->_readFds);
    this->_descriptors.insert(sock.get_fd());
}

void SocketsPool::addToWrite(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), &this->_writeFds);
    this->_descriptors.insert(sock.get_fd());
}

int SocketsPool::getMaxFd() const
{
    return *this->_descriptors.begin();
}

bool SocketsPool::isReadable(wsv::Socket const & sock) const
{
    for (int i = 0; i < getMaxFd() + 1; i++)
    {
        /* code */
    }
    
}

int SocketsPool::checkActivity()
{
    fd_set readSet, writeSet, excepSet;

    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&excepSet);
    FD_COPY(&this->_readFds, &readSet);
    FD_COPY(&this->_writeFds, &writeSet);
    FD_COPY(&this->_exepFds, &excepSet);
    int activity = select(getMaxFd() + 1, &readSet, &writeSet, &excepSet, &this->_timeout);
    return activity;
}

SocketsPool::~SocketsPool()
{
}