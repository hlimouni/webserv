#include "../headers/WebServer.hpp"
#define HELLO "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!"
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
                if (_pool.isReadable(*sock_it))
                {
                    acceptNewConnection(*sock_it);
                    std::cout << "New Accepted client socket fd: " << this->_clients.back().GetSocketFd() << std::endl;
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
                    if (this->recvRequest(cln_it) == false)
                    {
                        _clients.erase(cln_it);
                        continue;
                    }
                }
                if (_pool.isWriteable(currClient.GetSocket()))
                {
                    if (this->sendResponse(cln_it) == false)
                    {
                         _clients.erase(cln_it);
                         continue;
                    }
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
            perror("recv");
        }
        return false;
    }
    buff[nBytes] = 0;
    std::cout << "Received Bytes: " << nBytes << '\n';
    currClient.AppendToRequest(buff, nBytes);
    std::cout << "Received Message:\n"<< currClient.GetRequest() << std::endl;

    // currClient.SetSentBytes(0);

    if (this->isRequestValid(currClient))
    {
        if (currClient.IsChunked() == true)
            currClient.SetRequest(this->chunkedRequest(currClient));
        std::cout << "valid request\n";
        HttpRequest newReq;
        // newReq.setBuffer(std::string(buff));
        newReq.setBuffer(currClient.GetRequest());
        newReq.initRequest();
        HttpResponse resp(newReq, currClient.GetServerData());
        std::string respString = resp.getResponse();
        size_t respLen = respString.length();
        currClient.SetResponse(respString);
        currClient.SetTotalBytes(respLen);
        _pool.addToWrite(currClient.GetSocket());
    }
    return true;
}

bool WebServer::sendResponse(std::list<clientData>::iterator const & cln_it)
{
    clientData & currClient = *cln_it;
    std::cout << currClient.GetSocketFd() << "is writable\n";
    int nBytes(0);
    std::cout << currClient.GetTotalBytes() << ": total bytes\n";
    std::cout << currClient.GetSentBytes() << ": send bytes\n";
    if (currClient.GetTotalBytes() > currClient.GetSentBytes())
    {
        // HttpResponse resp(currClient.GetRequest(), currClient.GetServerData());
        // std::string respString = resp.getResponse();
        // size_t respLen = respString.length();
        std::cout << "response header: " << currClient.GetResponse().substr(0, 117) << '\n';
        std::cout << "sending\n";
        nBytes = send(currClient.GetSocketFd(),
                    (currClient.GetResponse().substr(currClient.GetSentBytes())).c_str(),
                    currClient.GetTotalBytes() - currClient.GetSentBytes(), 0);
        std::cout << currClient.GetTotalBytes() << ": total bytes\n";
        std::cout << nBytes << ": nBytes\n";
        // nBytes = send(currClient.GetSocketFd(),
        //             (respString.substr(currClient.GetSentBytes())).c_str(),
        //             600, 0);
        // nBytes = write(currClient.GetSocketFd(), HELLO, strlen(HELLO));
        // nBytes = send(currClient.GetSocketFd(),
        //             HELLO,
        //             MAX_BUFFER_LEN, 0);
        std::cout << "message send:\n";
        if (nBytes < 0)
        {
            std::cerr << "Couldnt's send data\n";
            // _clients.erase(cln_it);
            return false;
        }
        // _pending.find(currClient.GetListenFd())->second = false;


        close(currClient.GetSocketFd());
        // std::cout << currClient.GetSocketFd() << " is closed\n";
        _pool.removeFromRead(currClient.GetSocket());
        _pool.removeFromWrite(currClient.GetSocket());
        // _clients.erase(cln_it);
        return false;

        if (currClient.GetRequest().find("Connection: close") != std::string::npos)
        {
            close(currClient.GetSocketFd());
            return false;
        }

        // continue;
        if (nBytes == currClient.GetTotalBytes() - currClient.GetSentBytes())
        {
            currClient.SetTotalBytes(0);
            currClient.SetSentBytes(0);
            currClient.SetRequest("");
        }
        else
            currClient.IncrSentBytes(nBytes);
                    std::cout << currClient.GetTotalBytes() << ": total bytes\n";
        // std::cout << currClient.GetSentBytes() << ": sent bytes\n";
        
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
    // std::cout << acceptedSocket << "\n";
    if (acceptedSocket < 0)
        throw std::runtime_error("Counldn't accept connection");
    if (fcntl(acceptedSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Couldn't make accepted socket non blocking");
    wsv::Socket acceptedSockObj;
    acceptedSockObj.set_fd(acceptedSocket);
    acceptedSockObj.set_address(clientAddress);
    acceptedSockObj.set_len(addrlen);
    std::cout << "accepted socket: " << acceptedSockObj.get_fd() << '\n';

    for(std::vector<serverData>::iterator iter = this->_serverVect.begin();
        iter != this->_serverVect.end();
        iter++)
	{
        std::cout << iter->getHost() << '\n';
		if (iter->getHost() == "0.0.0.0" || iter->getHost() == inet_ntoa(clientAddress.sin_addr))
		{
            // std::cout << "Host found\n";
			std::set<int> ports = iter->getPorts();
			for(std::set<int>::iterator port_it = ports.begin(); port_it != ports.end(); port_it++)
			{
				if (*port_it == ntohs(sock.get_address().sin_port))
				{
                    this->_clients.push_back(clientData(acceptedSockObj, sock, *iter));
                    std::cout << "New Client Added to clients list\n";
                    // _pool.addToRead(acceptedSockObj);
					break;
				}
			}
		}
	}
    // acceptedObj.set_fd(acceptedSocket);
}

bool WebServer::isRequestValid(clientData & client)
{
    std::string buffer = client.GetRequest();
	if (buffer.find(D_CRLF) != std::string::npos)
	{
		std::string reqHeaders = buffer.substr(0, buffer.find(D_CRLF) + 4);
		if (reqHeaders.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			// this->_chunkedReq_ = true;
            std::cout << "Chunked\n";
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
