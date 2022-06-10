#ifndef __WEBSERVER_HPP
#define __WEBSERVER_HPP

#include "../headers/listeningSocket.hpp"
#include "../headers/SocketsPool.hpp"
#include "../headers/configParser.hpp"
#include "../headers/clientData.hpp"
#include <set>
#include <list>
#include <map>

class WebServer
{

private:
	// std::map< std::string, std::set<listeningSocket> > _hostSockets;
	std::map< std::string, std::list<listeningSocket> > _hostSockets;
    // std::set<listeningSocket> _listenSockets;
    std::map< int, bool> _handeledFds;
    std::list<listeningSocket> _listenSockets;
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