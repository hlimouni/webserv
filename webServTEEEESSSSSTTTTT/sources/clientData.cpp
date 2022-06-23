#include "../headers/clientData.hpp"

void clientData::SetTotalBytes(int n)
{
    _totalBytes = n;
}

int clientData::GetTotalBytes() const
{
    return _totalBytes;
}

void clientData::SetSentBytes(int n)
{
    _sentBytes = n;
}

void clientData::IncrSentBytes(int n)
{
    _sentBytes += n;
}

int clientData::GetSentBytes() const
{
    return _sentBytes;
}

void clientData::SetSocket(int s)
{
    _acceptedSocketFd = s;
}

int clientData::GetSocketFd() const
{
    return _acceptedSocketFd;
}

wsv::Socket const & clientData::GetSocket() const
{
    return _acceptedSocket;
}

void clientData::SetBuffer(char *szBuffer)
{
    strcpy(_szBuffer, szBuffer);
}

void clientData::GetBuffer(char *szBuffer)
{
    strcpy(szBuffer, _szBuffer);
}

char *clientData::GetBuffer() const
{
    return ((char *)_szBuffer);
}

int clientData::GetListenFd() const
{
    return _listenSocketFd;
}

size_t clientData::GetMaxBodySize() const
{
    return _maxBodySize;
}
// Constructor
// clientData::clientData(int acceptedSocketfd) : _totalBytes(0), _sentBytes(0)
// {
//     _acceptedSocketFd = acceptedSocketfd;
//     bzero(_szBuffer, MAX_BUFFER_LEN);
// }
clientData::clientData(wsv::Socket const & sock) : _totalBytes(0), _sentBytes(0), _acceptedSocket(sock), _isChunked(false)
{
    
    _acceptedSocketFd = sock.get_fd();
    bzero(_szBuffer, MAX_BUFFER_LEN);
}

clientData::clientData(wsv::Socket const & sock, wsv::Socket const & lstn)
    : _totalBytes(0), _sentBytes(0), _acceptedSocket(sock), _listenSocketFd(lstn.get_fd()), _isChunked(false)
{
    
    _acceptedSocketFd = sock.get_fd();
    bzero(_szBuffer, MAX_BUFFER_LEN);
}

clientData::clientData(wsv::Socket const & sock, wsv::Socket const & lstn, serverData & data)
    : _totalBytes(0), _sentBytes(0), _acceptedSocket(sock), _listenSocketFd(lstn.get_fd()), _serverData(data), _isChunked(false)
{
    _maxBodySize = data.getClientMaxBodySize() * 1024 * 1024;
    _acceptedSocketFd = sock.get_fd();
    bzero(_szBuffer, MAX_BUFFER_LEN);
}

std::string clientData::GetStringBuffer() const
{
    return std::string(_szBuffer);
}

void clientData::SetResponse(std::string const & resp)
{
    this->_response = resp;
}

std::string const & clientData::GetResponse() const
{
    return _response;
}

void clientData::SetRequest(std::string const & req)
{
    this->_request = req;
}

void clientData::AppendToRequest(char *str, int size)
{
    this->_request.append(str, size);
}

std::string const & clientData::GetRequest() const
{
    return this->_request;
}

serverData clientData::GetServerData() const
{
    return _serverData;
}

void clientData::SetChunkedValue(bool value)
{
    this->_isChunked = value;
}

bool clientData::IsChunked() const
{
    return _isChunked;
}

// destructor
clientData::~clientData()
{
    // close(_acceptedSocketFd);
}
