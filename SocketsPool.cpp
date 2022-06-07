#include "SocketsPool.hpp"

SocketsPool::SocketsPool()
{
    _timeout = (struct timeval){.tv_sec = 1, .tv_usec = 0};
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
    FD_ZERO(&this->_exepFds);
}

void SocketsPool::addToRead(wsv::Socket const &sock)
{
    FD_SET(sock.get_fd(), &this->_readFds);
    this->_descriptors.insert(sock.get_fd());
}

void SocketsPool::addToWrite(wsv::Socket const &sock)
{
    FD_SET(sock.get_fd(), &this->_writeFds);
    this->_descriptors.insert(sock.get_fd());
}

void SocketsPool::addToExept(wsv::Socket const &sock)
{
    FD_SET(sock.get_fd(), &this->_exepFds);
    this->_descriptors.insert(sock.get_fd());
}

int SocketsPool::getMaxFd() const
{
    return *this->_descriptors.begin();
}

bool SocketsPool::isReadable(wsv::Socket const &sock)
{
    if (FD_ISSET(sock.get_fd(), &this->_readFds))
        return true;
    return false;
}

bool SocketsPool::isWriteable(wsv::Socket const &sock)
{
    if (FD_ISSET(sock.get_fd(), &this->_writeFds))
        return true;
    return false;
}

void SocketsPool::initSets(std::map<std::string, std::set<listeningSocket> > hostSockets, std::list<clientData> clients)
{
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFds);
    FD_ZERO(&_exepFds);

    std::map<std::string, std::set<listeningSocket> >::iterator it;
    for (it = hostSockets.begin(); it != hostSockets.end(); it++)
    {
        std::set<listeningSocket> sockSet = it->second;

        for (std::set<listeningSocket>::iterator it_sock = sockSet.begin(); it_sock != sockSet.end(); it_sock++)
        {
            // FD_SET((*it_sock).get_fd(), &_readFds);
            // FD_SET((*it_sock).get_fd(), &_exepFds);
            addToRead(*it_sock);
            addToExept(*it_sock);
        }
    }
    for (std::list<clientData>::iterator it_client = clients.begin(); it_client != clients.end(); it_client++)
    {
        if ((*it_client).GetSentBytes() < (*it_client).GetTotalBytes())
            // FD_SET((*it_client).GetSocketFd(), &_writeFds);
            addToWrite((*it_client).GetSocket());
        else
            // FD_SET((*it_client).GetSocketFd(), &_readFds);
            addToRead((*it_client).GetSocket());
        // FD_SET((*it_client).GetSocketFd(), &_exepFds);
        addToExept((*it_client).GetSocket());
    }
}

int SocketsPool::checkActivity()
{
    int activity = select(getMaxFd() + 1, &_readFds, &_writeFds, &_exepFds,
                          &this->_timeout);
    return activity;
}

void SocketsPool::removeFromRead(wsv::Socket const &sock)
{
    FD_CLR(sock.get_fd(), &this->_readFds);
}

SocketsPool::~SocketsPool()
{
}
