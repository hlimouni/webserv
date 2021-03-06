#ifndef __SINGLESERVER_HPP
#define __SINGLESERVER_HPP

#include "listeningSocket.hpp"
#include "SocketsPool.hpp"
#include "headers/serverData.hpp"
#include "clientData.hpp"
#include <set>
#include <list>
#include <map>

class SingleServer
{

private:
	std::map<std::string, std::set<listeningSocket>> _hostSockets;
    std::set<listeningSocket> _listenSockets;
    std::list<clientData>     _clients;
    std::string               _host;
    SocketsPool               _pool;

    void acceptNewConnection(listeningSocket const & sock);

public:
    SingleServer(serverData & data);
    ~SingleServer();
};


#endif