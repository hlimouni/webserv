#ifndef __CLIENTDATA_HPP
#define __CLIENTDATA_HPP

#include "listeningSocket.hpp"
#include "configParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "serverData.hpp"
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
    size_t _maxBodySize;
    serverData _serverData;
    bool _isChunked;
    std::string _request;
    std::string _response;
    char _szBuffer[MAX_BUFFER_LEN];

public:
    // Get/Set calls
    void SetTotalBytes(int n);

    void SetRequest(std::string const &);
    void AppendToRequest(char *str, int bytes);
    std::string const & GetRequest() const;

    void SetResponse(std::string const &);
    std::string const & GetResponse() const;

    void SetChunkedValue(bool value);
    bool IsChunked() const;

    serverData GetServerData() const;

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
    std::string GetStringBuffer() const;
    size_t GetMaxBodySize() const;
    int GetListenFd() const;
    // Constructor
	clientData(wsv::Socket const & sock);
	clientData(wsv::Socket const & sock, wsv::Socket const & listn);
	clientData(wsv::Socket const & sock, wsv::Socket const & listn, serverData & data);
    // clientData(int acceptedSocketfd);
    // destructor
    ~clientData();
};

#endif