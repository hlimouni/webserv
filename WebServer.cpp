#include "WebServer.hpp"

WebServer::WebServer(configParser & parseData)
{
	std::vector<serverData>::iterator serv_it = parseData.getServers().begin();

	for ()
    this->_host = data.getHost();

    std::set<int> ports = data.getPorts();
    for (std::set<int>::iterator it = ports.begin(); it != ports.end(); it++)
    {
        listeningSocket sock(AF_INET, SOCK_STREAM, *it);
        this->_listenSockets.insert(sock);
        this->_pool.addToRead(sock);
    }

    while (true)
    {
        int activity = _pool.checkActivity();
        if (activity > 0)
        {
            std::set<listeningSocket>::iterator sock_it;
            for (sock_it = _listenSockets.begin(); sock_it != _listenSockets.end(); sock_it++)
            {
                if (_pool.isReadable(*sock_it) == true)
                    acceptNewConnection(*sock_it);
            }
        }
    }
}

WebServer::~WebServer()
{
}
