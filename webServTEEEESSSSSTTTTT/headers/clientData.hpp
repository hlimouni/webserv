#ifndef __CLIENTDATA_HPP
#define __CLIENTDATA_HPP

#include "../headers/listeningSocket.hpp"
#include "../headers/configParser.hpp"
#define MAX_BUFFER_LEN 1024
#include <unistd.h>
#include <string.h>

class clientData
{

private:
    int _totalBytes;
    int _sentBytes;
    wsv::Socket _acceptedSocket;
    int _acceptedSocketFd;
    int _listenSocketFd;
    char _szBuffer[MAX_BUFFER_LEN];

public:
    // Get/Set calls
    void SetTotalBytes(int n);

    int GetTotalBytes() const;
    void SetSentBytes(int n);
    void IncrSentBytes(int n);
    int GetSentBytes() const;
    void SetSocket(int s);
    int GetSocketFd() const;
    wsv::Socket const & GetSocket() const;
    void SetBuffer(char *szBuffer);
    void GetBuffer(char *szBuffer);
    char *GetBuffer() const;
    int GetListenFd() const;
    // Constructor
	clientData(wsv::Socket const & sock);
	clientData(wsv::Socket const & sock, wsv::Socket const & listn);
    // clientData(int acceptedSocketfd);
    // destructor
    ~clientData();
};

#endif