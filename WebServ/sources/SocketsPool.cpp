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
    // std::cout << "checking readability of " << sock.get_fd() << std::endl;
    if (FD_ISSET(sock.get_fd(), &this->_readFds))
    {
        // std::cout << "it is readable\n";
        return true;
    }
    // std::cout << "not redable\n";
    return false;
}

bool SocketsPool::isWriteable(wsv::Socket const & sock)
{
    // std::cout << "checking writablity of " << sock.get_fd() << std::endl;
    if (FD_ISSET(sock.get_fd(), &this->_writeFds))
    {
        // std::cout << "it is writable\n";
        return true;
    }
    // std::cout << "not writable\n";
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
        addToRead(*it_sock);
        addToExept(*it_sock);
    }
    for (std::list<clientData>::iterator it_client = clients.begin(); it_client != clients.end(); it_client++)
    {
		addToRead((*it_client).GetSocket());
        addToWrite((*it_client).GetSocket());
        // if ((*it_client).GetSentBytes() < (*it_client).GetTotalBytes())
        //     addToWrite((*it_client).GetSocket());
        // else
		// 	addToRead((*it_client).GetSocket());
		addToExept((*it_client).GetSocket());
    }
}

int SocketsPool::checkActivity()
{
    // std::cout << "maxfd: " << getMaxFd() << std::endl;
    _timeout = (struct timeval){.tv_sec = 1, .tv_usec = 0};

    int activity = select(getMaxFd() + 1, &_readFds, &_writeFds, &_exepFds,
                    0);
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
