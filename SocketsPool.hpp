#ifndef _SOCKETSPOOL_HPP
#define _SOCKETSPOOL_HPP

#include "listeningSocket.hpp"
#include "sys/select.h"
#include <set>
#include <list>
#include <map>
#include "clientData.hpp"

class SocketsPool
{

private:
    std::set<int, std::greater<int> > _descriptors;
    // fd_set _fdPool;
    fd_set _readFds;
    fd_set _writeFds;
    fd_set _exepFds;
    // fd_set _Current_readSet;
    // fd_set _Current_writeSet;
    // fd_set _Current_excepSet;
    struct timeval _timeout;

public:
    SocketsPool();
    SocketsPool(listeningSocket const &listenSock);
    ~SocketsPool();

    void initSets(std::set<listeningSocket> listenSockets, std::list<clientData> clients);
    // void initSets(std::set<listeningSocket> listenSockets, std::list<clientData> clients);
    void addToRead(wsv::Socket const & sock);
    void addToWrite(wsv::Socket const & sock);
    void addToExept(wsv::Socket const & sock);
    void removeFromRead(wsv::Socket const & sock);
    void removeFromWrite(wsv::Socket const & sock);
    bool isReadable(wsv::Socket const & sock);
    bool isWriteable(wsv::Socket const & sock);
    int getMaxFd() const;
    int checkActivity();

};


#endif