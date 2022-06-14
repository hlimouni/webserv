#include "../headers/WebServer.hpp"
#define HELLO "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"
#include "../headers/HttpRequest.hpp"
#include "../headers/HttpResponse.hpp"

WebServer::WebServer(configParser const &parseData)
{
    _serverVect = parseData.getServers();
    // _serverVect.assign(parseData.getServers().begin(), parseData.getServers().end());
    std::vector<serverData>::iterator it;

    for (it = _serverVect.begin(); it != _serverVect.end(); ++it)
    {
        std::set<int> ports = (*it).getPorts();
        std::list<listeningSocket> sockets;
        for (std::set<int>::iterator it = ports.begin(); it != ports.end(); it++)
        {
            listeningSocket sock(AF_INET, SOCK_STREAM, *it);
            sockets.push_back(sock);
            _listenSockets.push_back(sock);
            // _pending.insert(std::make_pair(sock.get_fd(), false));
            std::cout << "listening socket created: " << _listenSockets.back().get_fd() << std::endl;
        }
        this->_hostSockets.insert(std::make_pair((*it).getHost(), sockets));
        // this->_hostMaxClientSize.insert(std::make_pair((*it).getHost(), (*it).getClientMaxBodySize()));
    }
    this->startServer();
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
                if (_pool.isReadable(*sock_it) /* _pending.find((*sock_it).get_fd())->second == false*/)
                {
                    acceptNewConnection(*sock_it);
                    std::cout << "New Accepted client socket fd: " << this->_clients.back().GetSocketFd() << std::endl;
                    // _pool.addToWrite(*sock_it);
                    // _pending.find((*sock_it).get_fd())->second = true;
                }
                if (_pool.isExepted(*sock_it))
                    std::cerr << "Error while accepting Socket\n";
            }

            std::list<clientData>::iterator cln_it;
            for (cln_it = _clients.begin(); cln_it != _clients.end(); ++cln_it)
            {
                clientData & currClient = *cln_it;
                // std::cout << "current client socket fd is: " << currClient.GetSocketFd() << std::endl;
                if (_pool.isReadable(currClient.GetSocket()))
                {
                    if (this->recvRequest(cln_it) == false)
                    {
                        _clients.erase(cln_it);
                        continue;
                    }
                    
                    // std::cout << "Readable current socket: " << currClient.GetSocketFd() << '\n';
                    // // char buff[MAX_BUFFER_LEN];
                    // int nBytes = recv(currClient.GetSocketFd(), currClient.GetBuffer(), MAX_BUFFER_LEN, 0);
                    // // int nBytes = recv(currClient.GetSocketFd(), buff, MAX_BUFFER_LEN, 0);
                    // if (nBytes <= 0)
                    // {
                    //     if (nBytes)
                    //     {
                    //         perror("recv");
                    //         // std::cerr << "Couldn't receive socket data\n";
                    //     }
                    //     _clients.erase(cln_it);
                    //     continue;
                    // }
                    // currClient.GetBuffer()[nBytes] = 0;
                    // currClient.SetTotalBytes(nBytes);
                    // currClient.SetSentBytes(0);
                    // // std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
                    // std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
                    // // break;
                }
                if (_pool.isWriteable(currClient.GetSocket()))
                {
                    if (this->sendResponse(cln_it) == false)
                    {
                         _clients.erase(cln_it);
                         continue;
                    }
                    // std::cout << currClient.GetSocketFd() << "is writable\n";
                    // int nBytes(0);

                    // // std::cout << "Total Bytes: " << currClient.GetTotalBytes() << std::endl;
                    // // std::cout << "Sent Bytes: " << currClient.GetSentBytes() << std::endl;

                    // if (currClient.GetTotalBytes() > currClient.GetSentBytes())
                    // {
                        // nBytes = send(currClient.GetSocketFd(),
                        //             HELLO,
                        //             MAX_BUFFER_LEN, 0);
                        // // nBytes = write(currClient.GetSocketFd(), HELLO, strlen(HELLO));
                        // // nBytes = send(currClient.GetSocketFd(),
                        // //             currClient.GetBuffer() + currClient.GetSentBytes(),
                        // //             MAX_BUFFER_LEN, 0);
                        // std::cout << "message send\n";
                        // if (nBytes < 0)
                        // {
                        //     std::cerr << "Couldnt's send data\n";
                        //     _clients.erase(cln_it);
                        //     continue;
                        // }
                        // _pending.find(currClient.GetListenFd())->second = false;
                        // close(currClient.GetSocketFd());
                        // std::cout << currClient.GetSocketFd() << " is closed\n";
                        // _pool.removeFromRead(currClient.GetSocket());
                        // _pool.removeFromWrite(currClient.GetSocket());
                        // _clients.erase(cln_it);
                        // continue;
                        // if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
                        // {
                        //     currClient.SetTotalBytes(0);
                        //     currClient.SetSentBytes(0);

                        // }
                        // else
                        //     currClient.IncrSentBytes(nBytes);
                        // close(currClient.GetSocketFd());
                        // _pool.removeFromRead(currClient.GetSocket());
                    // }
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

bool WebServer::recvRequest(std::list<clientData>::iterator const & cln_it)
{
    clientData & currClient = *cln_it;
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
        // _clients.erase(cln_it);
        return false;
    }
    currClient.GetBuffer()[nBytes] = 0;
    std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
    currClient.SetTotalBytes(nBytes);
    currClient.SetSentBytes(0);
    _pool.addToWrite(currClient.GetSocket());

    if (this->isRequestValid(currClient))
    {
        HttpRequest newReq;
        newReq.setBuffer(currClient.GetStringBuffer());
        newReq.initRequest();
        currClient.SetRequest(newReq);
    }
    else
        return false;
    return true;
    // std::cout << "The following message was recieved: " << currClient.GetBuffer() << '\n';
}

bool WebServer::sendResponse(std::list<clientData>::iterator const & cln_it)
{
    clientData & currClient = *cln_it;
    std::cout << currClient.GetSocketFd() << "is writable\n";
    int nBytes(0);
    // std::cout << "Total Bytes: " << currClient.GetTotalBytes() << std::endl;
    // std::cout << "Sent Bytes: " << currClient.GetSentBytes() << std::endl;
    if (currClient.GetTotalBytes() > currClient.GetSentBytes())
    {
        HttpResponse resp(currClient.GetRequest(), currClient.GetServerData());
        std::string respString = resp.getResponse();
        size_t respLen = respString.length();
        nBytes = send(currClient.GetSocketFd(),
                    respString.c_str(),
                    respLen, 0);
        // nBytes = write(currClient.GetSocketFd(), HELLO, strlen(HELLO));
        // nBytes = send(currClient.GetSocketFd(),
        //             HELLO,
        //             MAX_BUFFER_LEN, 0);
        std::cout << "message send:\n";
        std::cout << respString.substr(0, 200) << "\n";
        if (nBytes < 0)
        {
            std::cerr << "Couldnt's send data\n";
            // _clients.erase(cln_it);
            return false;
        }
        // _pending.find(currClient.GetListenFd())->second = false;

        /*
        close(currClient.GetSocketFd());
        std::cout << currClient.GetSocketFd() << " is closed\n";
        _pool.removeFromRead(currClient.GetSocket());
        _pool.removeFromWrite(currClient.GetSocket());
        // _clients.erase(cln_it);
        return false;
        */




        currClient.SetTotalBytes(0);
        currClient.SetSentBytes(0);
        // continue;
        // if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
        // {
        //     currClient.SetTotalBytes(0);
        //     currClient.SetSentBytes(0);
        // }
        // else
        //     currClient.IncrSentBytes(nBytes);
        // close(currClient.GetSocketFd());
        // _pool.removeFromRead(currClient.GetSocket());
    }
    return true;
}

void WebServer::acceptNewConnection(listeningSocket const & sock)
{
    sockaddr_in clientAddress;
    size_t addrlen = sizeof(clientAddress);
    int acceptedSocket = accept(sock.get_fd(), (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
    std::cout << acceptedSocket << "\n";
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");

        //keep-alive
    // int yes = 1;
    // if (setsockopt(acceptedSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) < 0)
    //     throw std::runtime_error("Couldn't set keep-alive option for socket");
    int yes = 1;
    if (setsockopt(
        acceptedSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1)
        throw std::runtime_error("keepalive");

    // int idle = 1;
    // if (setsockopt(
    //     acceptedSocket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) == -1)
    //     throw std::runtime_error("keep-alive");

    int interval = 1;
    if (setsockopt(
        acceptedSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) == -1)
        throw std::runtime_error("keep-alive");

    int maxpkt = 10;
    if (setsockopt(
        acceptedSocket, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) == -1)
        throw std::runtime_error("keep-alive");


    wsv::Socket acceptedSockObj;
    acceptedSockObj.set_fd(acceptedSocket);
    acceptedSockObj.set_address(clientAddress);
    acceptedSockObj.set_len(addrlen);
    std::cout << "accepted socket: " << acceptedSockObj.get_fd() << '\n';

    for(std::vector<serverData>::iterator iter = this->_serverVect.begin();
        iter != this->_serverVect.end();
        iter++)
	{
		// if (iter->getHost() == inet_ntoa(clientAddress.sin_addr))
		// {
            // std::cout << "Host found\n";
			std::set<int> ports = iter->getPorts();
			for(std::set<int>::iterator port_it = ports.begin(); port_it != ports.end(); port_it++)
			{
				if (*port_it == ntohs(sock.get_address().sin_port))
				{
                    this->_clients.push_back(clientData(acceptedSockObj, sock, *iter));
                    std::cout << "New Client Added to clients list\n";
					break;
				}
                else
                {
                    std::cout << *port_it << " != " << ntohs(sock.get_address().sin_port) << '\n';
                }
			}
		// }
	}
    // acceptedObj.set_fd(acceptedSocket);
}

bool WebServer::isRequestValid(clientData const & client)
{
    std::string buffer = client.GetStringBuffer();
	if (buffer.find(D_CRLF) != std::string::npos)
	{
		std::string reqHeaders = buffer.substr(0, buffer.find(D_CRLF) + 4);
		if (reqHeaders.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			// this->_chunkedReq_ = true;
            std::cout << "Chunked\n";
			if (buffer.find(LAST_CHUNK) != std::string::npos)
				return true;
			return false;
		}
		else if (reqHeaders.find("Content-Length") != std::string::npos)
		{
			size_t bodySize = std::stoi(reqHeaders.substr((int)(reqHeaders.find("Content-Length: ")) + 16));
			std::string reqBody = buffer.substr(buffer.find(D_CRLF) + 4);
	
			if (bodySize > client.GetMaxBodySize())
				return true;
			else if (reqBody.length() < bodySize)
				return false;
		}
		return true;
	}
	return false;
}

WebServer::~WebServer()
{
}
