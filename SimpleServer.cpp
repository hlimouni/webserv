#include "SimpleServer.hpp"

WSV::SimpleServer::SimpleServer(Socket const &sockListen)
{
    this->_sockListen = sockListen;
}

void WSV::SimpleServer::listen_for_reqs(int backlog)
{
    if (listen(_sockListen.get_fd(), backlog) < 0)
    {
        std::cerr << "server couldn't listen to port" << '\n';
        exit(EXIT_FAILURE);
    }

}

void WSV::SimpleServer::accept_req()
{
    int addrlen = _sockListen.len();
    this->_sockexchangeFd = accept(_sockListen.get_fd(), (struct sockaddr *)&_sockListen, (socklen_t *)&addrlen);
    if (_sockexchangeFd < 0)
    {
        std::cerr << "couldn't accept connection" << '\n';
        exit(EXIT_FAILURE);
    }
}

WSV::SimpleServer::~SimpleServer()
{
}
