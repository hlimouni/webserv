#include "WebServer.hpp"

WebServer::WebServer(configParser const &parseData)
{
    std::vector<serverData> serverVect = parseData.getServers();
    std::vector<serverData>::iterator it;

    for (it = serverVect.begin(); it != serverVect.end(); ++it)
    {

        std::set<int> ports = (*it).getPorts();
        std::set<listeningSocket> sockets;
        for (std::set<int>::iterator it = ports.begin(); it != ports.end(); it++)
        {
            listeningSocket sock(AF_INET, SOCK_STREAM, *it);
            sockets.insert(sock);
            _listenSockets.insert(sock);
        }
        this->_hostSockets.insert(std::make_pair((*it).getHost(), sockets));
    }
    this->startServer();
    // while (true)
    // {
    //     int activity = _pool.checkActivity();
    //     if (activity > 0)
    //     {
    //         std::set<listeningSocket>::iterator sock_it;
    //         for (sock_it = _listenSockets.begin(); sock_it != _listenSockets.end(); sock_it++)
    //         {
    //             if (_pool.isReadable(*sock_it) == true)
    //                 acceptNewConnection(*sock_it);
    //         }
    //     }
    // }
}

void WebServer::startServer()
{
    this->_pool.initSets(_listenSockets, _clients);
    while (true)
    {
        int activity = _pool.checkActivity();
        if (activity > 0)
        {
            std::set<listeningSocket>::iterator sock_it;
            for (sock_it = _listenSockets.begin(); sock_it != _listenSockets.end(); ++sock_it)
            {
                if (_pool.isReadable(*sock_it))
                    acceptNewConnection(*sock_it);
                if (_pool.isExepted(*sock_it))
                    std::cerr << "Error while accepting Socket\n";
            }

            std::list<clientData>::iterator cln_it;
            for (cln_it = _clients.begin(); cln_it != _clients.end(); ++cln_it)
            {
                clientData & currClient = *cln_it;
                if (_pool.isReadable(currClient.GetSocket()))
                {
                    int nBytes = recv(currClient.GetSocketFd(), currClient.GetBuffer(), MAX_BUFFER_LEN, 0);
                    if (nBytes <= 0)
                    {
                        if (nBytes)
                            std::cerr << "Couldn't receive socket data\n";
                        _clients.erase(cln_it);
                        continue;
                    }
                    currClient.SetTotalBytes(nBytes);
                    currClient.SetSentBytes(0);
                    std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
                }
                if (_pool.isWriteable(currClient.GetSocket()))
                {
                    int nBytes(0);

                    if (currClient.GetTotalBytes() < currClient.GetSentBytes())
                    {
                        nBytes = send(currClient.GetSocketFd(),
                                    currClient.GetBuffer() + currClient.GetSentBytes(),
                                    MAX_BUFFER_LEN, 0);
                        if (nBytes < 0)
                        {
                            std::cerr << "Couldnt's send data\n";
                            _clients.erase(cln_it);
                            continue;
                        }
                        if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
                        {
                            currClient.SetTotalBytes(0);
                            currClient.SetSentBytes(0);
                        }
                        else
                            currClient.IncrSentBytes(nBytes);
                    }
                }
                if (_pool.isExepted(currClient.GetSocket()))
                {
                    std::cerr << "Error on socket\n";
                    _clients.erase(cln_it);
                }
            }
        }
        else
            std::cerr << "select error\n";
    }
    //closing the open sockets (to be implemented in the destructor for socketsPool)
}

void WebServer::acceptNewConnection(listeningSocket const & sock )
{
    int addrlen = sock.len();
    int acceptedSocket = accept(sock.get_fd(), (struct sockaddr *)&sock.get_address(), (socklen_t *)&addrlen);
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");
    // clientData newClient(acceptedSocket);
    this->_clients.push_back(clientData(sock));
}

WebServer::~WebServer()
{
}
