#ifndef __CLIENTDATA_HPP
#define __CLIENTDATA_HPP

#include "../headers/listeningSocket.hpp"
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

    int GetTotalBytes();
    void SetSentBytes(int n);
    void IncrSentBytes(int n);
    int GetSentBytes();
    void SetSocket(int s);
    int GetSocketFd();
    wsv::Socket const & GetSocket();
    void SetBuffer(char *szBuffer);
    void GetBuffer(char *szBuffer);
    char *GetBuffer();
    int GetListenFd();
    // Constructor
	clientData(wsv::Socket const & sock);
	clientData(wsv::Socket const & sock, wsv::Socket const & listn);
    // clientData(int acceptedSocketfd);
    // destructor
    ~clientData();
};

#endif