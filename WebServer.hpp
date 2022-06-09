#ifndef __WEBSERVER_HPP
#define __WEBSERVER_HPP

#include "listeningSocket.hpp"
#include "SocketsPool.hpp"
#include "headers/configParser.hpp"
#include "clientData.hpp"
#include <set>
#include <list>
#include <map>

class WebServer
{

private:
	std::map< std::string, std::set<listeningSocket> > _hostSockets;
    std::set<listeningSocket> _listenSockets;
    std::list<clientData>     _clients;
    std::string               _host;
    SocketsPool               _pool;

public:
	WebServer(configParser const & parseData);
	~WebServer();

    void startServer();
    void acceptNewConnection(listeningSocket const & sock);
};

#endif