#include "../headers/WebServer.hpp"
#include "../headers/HttpRequest.hpp"
#include "../headers/HttpResponse.hpp"

WebServer::WebServer(configParser const &parseData)
{
    _serverVect = parseData.getServers();
    std::vector<serverData>::iterator it;

    for (it = _serverVect.begin(); it != _serverVect.end(); ++it)
    {
        std::set<int> ports = (*it).getPorts();
        std::list<listeningSocket> sockets;
        for (std::set<int>::iterator ports_it = ports.begin(); ports_it != ports.end(); ports_it++)
        {
            listeningSocket sock(AF_INET, SOCK_STREAM, *ports_it);
            sockets.push_back(sock);
            _listenSockets.push_back(sock);
            std::cout << "listening socket created with fd: " << sock.get_fd() << " bound to host " << (*it).getHost() << ':' << (*ports_it) << std::endl;
        }
        this->_hostSockets.insert(std::make_pair((*it).getHost(), sockets));
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
                if (_pool.isReadable(*sock_it))
                {
                    acceptNewConnection(*sock_it);
                    // std::cout << "New Accepted client socket fd: " << this->_clients.back().GetSocketFd() << std::endl;
                }
                if (_pool.isExepted(*sock_it))
                    std::cerr << "Error while accepting Socket\n";
            }

            std::list<clientData>::iterator cln_it;
            for (cln_it = _clients.begin(); cln_it != _clients.end(); ++cln_it)
            {
                clientData & currClient = *cln_it;
                // std::cout << "current client socket fd is: " << currClient.GetSocketFd() << std::endl;
                if (_pool.isReadable(currClient.GetSocket()) && _pool.isWriteable(currClient.GetSocket()))
                {
                    if (this->recvRequest(cln_it) == false)
                    {
                        _clients.erase(cln_it);
                        continue;
                    }
                    if (this->sendResponse(cln_it) == false)
                    {
                         _clients.erase(cln_it);
                         continue;
                    }
                }
                // check for reading and writing seperately
                // ********************************
                //
                // if (_pool.isReadable(currClient.GetSocket()))
                // {
                //     if (this->recvRequest(cln_it) == false)
                //     {
                //         _clients.erase(cln_it);
                //         continue;
                //     }
                // }
                // if (_pool.isWriteable(currClient.GetSocket()))
                // {
                //     if (this->sendResponse(cln_it) == false)
                //     {
                //          _clients.erase(cln_it);
                //          continue;
                //     }
                // }
                if (_pool.isExepted(currClient.GetSocket()))
                {
                    std::cerr << "Error on socket\n";
                    _clients.erase(cln_it);
                }
            }
        }
        else if (activity < 0)
        {
            std::cout << "select failed\n";
        }
    }

    //closing the open sockets (to be implemented in the destructor for socketsPool)
}

bool WebServer::recvRequest(std::list<clientData>::iterator const & cln_it)
{
    clientData & currClient = *cln_it;
    char buff[MAX_BUFFER_LEN + 1];
    int nBytes = recv(currClient.GetSocketFd(), buff, MAX_BUFFER_LEN, 0);
    if (nBytes <= 0)
    {
        if (nBytes)
        {
            std::cerr << "couldn't receive data from client\n";
        }
        return false;
    }
    buff[nBytes] = 0;
    // std::cout << "Received Bytes: " << nBytes << '\n';
    currClient.AppendToRequest(buff, nBytes);
    // std::cout << "Received Message:\n"<< currClient.GetRequest() << std::endl;
    std::cout << "Received data from client on socket: " << currClient.GetSocketFd() << " with IP address " << inet_ntoa(currClient.GetSocket().get_address().sin_addr) << ':' << ntohs(currClient.GetSocket().get_address().sin_port) << std::endl;
    if (this->isRequestValid(currClient))
    {
        if (currClient.IsChunked() == true)
            currClient.SetRequest(this->chunkedRequest(currClient));
        HttpRequest newReq;
        newReq.setBuffer(currClient.GetRequest());
        newReq.initRequest();
        HttpResponse resp(newReq, currClient.GetServerData());
        std::string respString = resp.getResponse();
        size_t respLen = respString.length();
        currClient.SetResponse(respString);
        currClient.SetTotalBytes(respLen);
        // _pool.addToWrite(currClient.GetSocket());
    }
    return true;
}

bool WebServer::sendResponse(std::list<clientData>::iterator const & cln_it)
{
    clientData & currClient = *cln_it;
    int nBytes(0);

    // std::cout << currClient.GetTotalBytes() << ": total bytes\n";
    // std::cout << currClient.GetSentBytes() << ": send bytes\n";
    if (currClient.GetTotalBytes() > currClient.GetSentBytes())
    {
        nBytes = send(currClient.GetSocketFd(),
                    (currClient.GetResponse().substr(currClient.GetSentBytes())).c_str(),
                    currClient.GetTotalBytes() - currClient.GetSentBytes(), 0);
        if (nBytes < 0)
        {
            std::cerr << "Couldnt's send data\n";
            return false;
        }
        std::cout << "Response sent on socket: " << currClient.GetSocketFd() << std::endl;
        close(currClient.GetSocketFd());
        _pool.removeFromRead(currClient.GetSocket());
        _pool.removeFromWrite(currClient.GetSocket());
        return false;

        //persistence
        //***********

        // if (currClient.GetRequest().find("Connection: close") != std::string::npos)
        // {
        //     close(currClient.GetSocketFd());
        //     return false;
        // }
        // if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
        // {
        //     currClient.SetTotalBytes(0);
        //     currClient.SetSentBytes(0);
        //     currClient.SetRequest("");
        // }
        // else
        //     currClient.IncrSentBytes(nBytes);
        //             std::cout << currClient.GetTotalBytes() << ": total bytes\n";
    }
    return true;
}

void WebServer::acceptNewConnection(listeningSocket const & sock)
{
    sockaddr_in clientAddress;
    size_t addrlen = sizeof(clientAddress);
    int acceptedSocket = accept(sock.get_fd(), (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    std::cout << "New connection established through listening socket: " << sock.get_fd() << ", with accepted fd: " << acceptedSocket << ", Address/port: " << inet_ntoa(clientAddress.sin_addr) << ':' << ntohs(clientAddress.sin_port) << '\n';
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");
    wsv::Socket acceptedSockObj;
    acceptedSockObj.set_fd(acceptedSocket);
    acceptedSockObj.set_address(clientAddress);
    acceptedSockObj.set_len(addrlen);

    for(std::vector<serverData>::iterator iter = this->_serverVect.begin();
        iter != this->_serverVect.end();
        iter++)
	{
		if (iter->getHost() == inet_ntoa(clientAddress.sin_addr))
		{
			std::set<int> ports = iter->getPorts();
			for(std::set<int>::iterator port_it = ports.begin(); port_it != ports.end(); port_it++)
			{
				if (*port_it == ntohs(sock.get_address().sin_port))
				{
                    this->_clients.push_back(clientData(acceptedSockObj, sock, *iter));
					break;
				}
			}
		}
	}
}

bool WebServer::isRequestValid(clientData & client)
{
    std::string buffer = client.GetRequest();
	if (buffer.find(D_CRLF) != std::string::npos)
	{
		std::string reqHeaders = buffer.substr(0, buffer.find(D_CRLF) + 4);
		if (reqHeaders.find("Transfer-Encoding: chunked") != std::string::npos)
		{
            client.SetChunkedValue(true);
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

std::string const WebServer::chunkedRequest(clientData & client)
{
    std::string chunkReq = client.GetRequest();
	std::string fusedRequest =chunkReq.substr(0,chunkReq.find(D_CRLF) + 4);
	std::string requestBody =chunkReq.substr(chunkReq.find(D_CRLF) + 4);
	size_t i = 0;
	while (i < requestBody.size())
	{
		std::string chunks = requestBody.substr(i, std::string::npos);
		std::stringstream chunksStream(chunks);
		std::string line;
		std::getline(chunksStream, line);
		i += line.length() + 1;
		size_t chunkLen = this->getChunkSize(line);
		fusedRequest.append(requestBody.c_str() + i, chunkLen);
		i+= chunkLen + 2;
	}
    client.SetChunkedValue(false);
    std::cout << "fused request:\n"<< fusedRequest << '\n';
	return (fusedRequest);
}

size_t WebServer::getChunkSize(std::string & line)
{
    size_t chunkSize(0);
    line.pop_back();
    if (line.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
        throw std::runtime_error("Wrong Format for chunk size");
    std::stringstream chunkSizeStream(line);
    chunkSizeStream >> std::hex >> chunkSize;
    return chunkSize;
}

WebServer::~WebServer()
{
}
