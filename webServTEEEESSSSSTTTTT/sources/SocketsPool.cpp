#include "../headers/SocketsPool.hpp"

SocketsPool::SocketsPool()
{
    _timeout = (struct timeval){.tv_sec = 1, .tv_usec = 0};
    FD_ZERO(&this->_readFds);
    FD_ZERO(&this->_writeFds);
    FD_ZERO(&this->_exepFds);
}

void SocketsPool::addToRead(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), &this->_readFds);
    std::cout << "socket added to read: " << sock.get_fd() << "\n";
    this->_descriptors.insert(sock.get_fd());
}

void SocketsPool::addToWrite(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), &this->_writeFds);
    this->_descriptors.insert(sock.get_fd());
}

void SocketsPool::addToExept(wsv::Socket const & sock)
{
    FD_SET(sock.get_fd(), &this->_exepFds);
    this->_descriptors.insert(sock.get_fd());
}

int SocketsPool::getMaxFd() const
{
    return *this->_descriptors.begin();
}

bool SocketsPool::isReadable(wsv::Socket const & sock)
{
    std::cout << "checking readability of: " << sock.get_fd() << std::endl;
    if (FD_ISSET(sock.get_fd(), &this->_readFds))
    {
        std::cout << "it is readable\n";
        return true;
    }
    std::cout << "not readable\n";
    return false;
}

bool SocketsPool::isWriteable(wsv::Socket const & sock)
{
    if (FD_ISSET(sock.get_fd(), &this->_writeFds))
        return true;
    return false;
}

bool SocketsPool::isExepted(wsv::Socket const & sock)
{
    if (FD_ISSET(sock.get_fd(), &this->_exepFds))
        return true;
    return false;
}

void SocketsPool::initSets(std::list<listeningSocket> listenSockets, std::list<clientData> clients)
{
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFds);
    FD_ZERO(&_exepFds);

    this->_descriptors.clear();
    for (std::list<listeningSocket>::iterator it_sock = listenSockets.begin(); it_sock != listenSockets.end(); it_sock++)
    {
        // FD_SET((*it_sock).get_fd(), &_readFds);
        // FD_SET((*it_sock).get_fd(), &_exepFds);
        addToRead(*it_sock);
        addToExept(*it_sock);
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
    std::cout << "maxfd: " << getMaxFd() << std::endl;
    int activity = select(getMaxFd() + 1, &_readFds, &_writeFds, &_exepFds,
                    &this->_timeout);
    return activity;
}

void SocketsPool::removeFromRead(wsv::Socket const & sock)
{
    FD_CLR(sock.get_fd(), &this->_readFds);
}

void SocketsPool::removeFromWrite(wsv::Socket const & sock)
{
    FD_CLR(sock.get_fd(), &this->_writeFds);
}

SocketsPool::~SocketsPool()
{
}
