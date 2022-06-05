#include "SingleServer.hpp"

SingleServer::SingleServer(serverData & data)
{
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

void SingleServer::acceptNewConnection(listeningSocket const & sock)
{
    int addrlen = sock.len();
    int acceptedSocket = accept(sock.get_fd(), (struct sockaddr *)&sock.get_address(), (socklen_t *)&addrlen);
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");
    // clientData newClient(acceptedSocket);
    this->_clients.push_back(clientData(acceptedSocket));

}

SingleServer::~SingleServer()
{
}
