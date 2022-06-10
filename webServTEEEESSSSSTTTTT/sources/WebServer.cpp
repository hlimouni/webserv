#include "../headers/WebServer.hpp"
#define HELLO "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"

WebServer::WebServer(configParser const &parseData)
{
    std::vector<serverData> serverVect = parseData.getServers();
    std::vector<serverData>::iterator it;

    for (it = serverVect.begin(); it != serverVect.end(); ++it)
    {
        std::set<int> ports = (*it).getPorts();
        std::list<listeningSocket> sockets;
        for (std::set<int>::iterator it = ports.begin(); it != ports.end(); it++)
        {
            listeningSocket sock(AF_INET, SOCK_STREAM, *it);
            sockets.push_back(sock);
            _listenSockets.push_back(sock);
            _handeledFds.insert(std::make_pair(sock.get_fd(), false));
            std::cout << "listening socket created: " << _listenSockets.back().get_fd() << std::endl;
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
    while (true)
    {
        this->_pool.initSets(_listenSockets, _clients);
        int activity = _pool.checkActivity();
        if (activity > 0)
        {
            std::list<listeningSocket>::iterator sock_it;
            for (sock_it = _listenSockets.begin(); sock_it != _listenSockets.end(); ++sock_it)
            {
                if (_pool.isReadable(*sock_it) && _handeledFds.find((*sock_it).get_fd())->second == false)
                {
                    acceptNewConnection(*sock_it);
                    std::cout << "New Accepted client socket fd: " << this->_clients.back().GetSocketFd() << std::endl;
                    _pool.addToWrite(*sock_it);
                    _handeledFds.find((*sock_it).get_fd())->second = true;
                }
                if (_pool.isExepted(*sock_it))
                    std::cerr << "Error while accepting Socket\n";
            }

            std::list<clientData>::iterator cln_it;
            for (cln_it = _clients.begin(); cln_it != _clients.end(); ++cln_it)
            {
                clientData & currClient = *cln_it;
                std::cout << "current client socket fd is: " << currClient.GetSocketFd() << std::endl;
                if (_pool.isReadable(currClient.GetSocket()))
                {
                    std::cout << "Readable current socket: " << currClient.GetSocketFd() << '\n';
                    // char buff[MAX_BUFFER_LEN];
                    int nBytes = recv(currClient.GetSocketFd(), currClient.GetBuffer(), MAX_BUFFER_LEN, 0);
                    // int nBytes = recv(currClient.GetSocketFd(), buff, MAX_BUFFER_LEN, 0);
                    if (nBytes <= 0)
                    {
                        if (nBytes)
                        {
                            perror("recv");
                            // std::cerr << "Couldn't receive socket data\n";
                        }
                        _clients.erase(cln_it);
                        continue;
                    }
                    currClient.SetTotalBytes(nBytes);
                    currClient.SetSentBytes(0);
                    // std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
                    std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
                    // break;
                }
                if (_pool.isWriteable(currClient.GetSocket()))
                {
                    std::cout << currClient.GetSocketFd() << "is writable\n";
                    int nBytes(0);

                    // std::cout << "Total Bytes: " << currClient.GetTotalBytes() << std::endl;
                    // std::cout << "Sent Bytes: " << currClient.GetSentBytes() << std::endl;

                    if (currClient.GetTotalBytes() > currClient.GetSentBytes())
                    {
                        nBytes = send(currClient.GetSocketFd(),
                                    HELLO,
                                    MAX_BUFFER_LEN, 0);
                        // nBytes = write(currClient.GetSocketFd(), HELLO, strlen(HELLO));
                        // nBytes = send(currClient.GetSocketFd(),
                        //             currClient.GetBuffer() + currClient.GetSentBytes(),
                        //             MAX_BUFFER_LEN, 0);
                        std::cout << "message send\n";
                        if (nBytes < 0)
                        {
                            std::cerr << "Couldnt's send data\n";
                            _clients.erase(cln_it);
                            continue;
                        }
                             _handeledFds.find(currClient.GetListenFd())->second = false;
                            close(currClient.GetSocketFd());
                            std::cout << currClient.GetSocketFd() << " is closed\n";
                            _pool.removeFromRead(currClient.GetSocket());
                            _pool.removeFromWrite(currClient.GetSocket());
                            _clients.erase(cln_it);
                            continue;
                        if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
                        {
                            currClient.SetTotalBytes(0);
                            currClient.SetSentBytes(0);

                        }
                        else
                            currClient.IncrSentBytes(nBytes);
                        // close(currClient.GetSocketFd());
                        // _pool.removeFromRead(currClient.GetSocket());
                    }
                    // exit(0);
                }
                if (_pool.isExepted(currClient.GetSocket()))
                {
                    std::cerr << "Error on socket\n";
                    _clients.erase(cln_it);
                }
            }
        }
        else if (activity < 0)
        {
            perror("select");
            sleep(1);
            // std::cerr << "select error\n";
        }

    }

    //closing the open sockets (to be implemented in the destructor for socketsPool)
}

void WebServer::acceptNewConnection(listeningSocket const & sock )
{
    sockaddr_in clientAddress;
    size_t addrlen = sizeof(clientAddress);
    int acceptedSocket = accept(sock.get_fd(), (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
    std::cout << acceptedSocket << "\n";
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");
    wsv::Socket acceptedSockObj;
    acceptedSockObj.set_fd(acceptedSocket);
    acceptedSockObj.set_address(clientAddress);
    acceptedSockObj.set_len(addrlen);
    std::cout << "accepted socket: " << acceptedSockObj.get_fd() << '\n';

    // acceptedObj.set_fd(acceptedSocket);
    this->_clients.push_back(clientData(acceptedSockObj, sock));
}

WebServer::~WebServer()
{
}
