#ifndef __WEBSERVER_HPP
#define __WEBSERVER_HPP

#include "../headers/listeningSocket.hpp"
#include "../headers/SocketsPool.hpp"
#include "../headers/configParser.hpp"
#include "../headers/clientData.hpp"
#include <arpa/inet.h>
#include <sys/poll.h>
#include <netinet/tcp.h>
#include <set>
#include <list>
#include <map>

# define LAST_CHUNK "0\r\n\r\n"
# define CRLF "\r\n"
# define D_CRLF "\r\n\r\n"

class WebServer
{

private:
	// std::map< std::string, std::set<listeningSocket> > _hostSockets;
	std::map< std::string, std::list<listeningSocket> > _hostSockets;
    // std::map< std::string, int> _hostMaxClientSize;
    // std::set<listeningSocket> _listenSockets;
    std::vector<serverData> _serverVect;
    // std::map< int, bool> _pending;
    std::list<listeningSocket> _listenSockets;
    std::list<clientData>     _clients;
    std::string               _host;
    SocketsPool               _pool;

public:
	WebServer(configParser const & parseData);
	~WebServer();

    void startServer();
    void acceptNewConnection(listeningSocket const & sock);
    bool recvRequest(std::list<clientData>::iterator const & cln_it);
    bool sendResponse(std::list<clientData>::iterator const & cln_it);
    bool isRequestValid(clientData & client);
    std::string const chunkedRequest(clientData & client);
    size_t getChunkSize(std::string & line);
};

#endif