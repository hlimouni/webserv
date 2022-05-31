#ifndef _SOCKETSPOOL_HPP
#define _SOCKETSPOOL_HPP

#include "listeningSocket.hpp"
#include <set>

class SocketsPool
{

private:
    std::set<int, std::greater<int> > _descriptors;
    fd_set _fdPool;
    fd_set _readFds;
    fd_set _writeFds;
    struct timeval _timeout;
    SocketsPool();

public:
    SocketsPool(listeningSocket const &listenSock);
    ~SocketsPool();

    void addToRead(wsv::Socket const & sock);
    void addToWrite(wsv::Socket const & sock);
    void remove(wsv::Socket const & sock);
    bool isRead();
    bool isWrite();
};


#endif