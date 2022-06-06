#include "clientData.hpp"

void clientData::SetTotalBytes(int n)
{
    _totalBytes = n;
}

int clientData::GetTotalBytes()
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

int clientData::GetSentBytes()
{
    return _sentBytes;
}

void clientData::SetSocket(int s)
{
    _acceptedSocketFd = s;
}

int clientData::GetSocketFd()
{
    return _acceptedSocketFd;
}

void clientData::SetBuffer(char *szBuffer)
{
    strcpy(_szBuffer, szBuffer);
}

void clientData::GetBuffer(char *szBuffer)
{
    strcpy(szBuffer, _szBuffer);
}

char *clientData::GetBuffer()
{
    return _szBuffer;
}
// Constructor
// clientData::clientData(int acceptedSocketfd) : _totalBytes(0), _sentBytes(0)
// {
//     _acceptedSocketFd = acceptedSocketfd;
//     bzero(_szBuffer, MAX_BUFFER_LEN);
// }
clientData::clientData(wsv::Socket const & sock) : _totalBytes(0), _sentBytes(0), _acceptedSocket(sock)
{
    
    _acceptedSocketFd = sock.get_fd();
    bzero(_szBuffer, MAX_BUFFER_LEN);
}
// destructor
clientData::~clientData()
{
    close(_acceptedSocketFd);
}
