#ifndef _SOCKETSPOOL_HPP
#define _SOCKETSPOOL_HPP

#include "listeningSocket.hpp"
#include "sys/select.h"
#include <set>

class SocketsPool
{

private:
    std::set<int, std::greater<int> > _descriptors;
    fd_set _fdPool;
    fd_set _readFds;
    fd_set _writeFds;
    fd_set _exepFds;
    struct timeval _timeout;

public:
    SocketsPool();
    SocketsPool(listeningSocket const &listenSock);
    ~SocketsPool();

    void addToRead(wsv::Socket const & sock);
    void addToWrite(wsv::Socket const & sock);
    void remove(wsv::Socket const & sock);
    bool isReadable(wsv::Socket const & sock) const;
    bool isWriteable(wsv::Socket const & sock) const;
    int getMaxFd() const;
    int checkActivity();

};


#endif