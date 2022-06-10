#include "../headers/SimpleServer.hpp"

wsv::SimpleServer::SimpleServer(Socket const &sockListen)
{
    this->addrlen = sockListen.len();
    this->_sockListen = sockListen;
}

void wsv::SimpleServer::listen_for_reqs(int backlog)
{
    if (listen(_sockListen.get_fd(), backlog) < 0)
    {
        std::cerr << "server couldn't listen to port" << '\n';
        exit(EXIT_FAILURE);
    }
}

void wsv::SimpleServer::accept_req()
{
    int addrlen = _sockListen.len();
    this->_sockexchangeFd = accept(_sockListen.get_fd(), (struct sockaddr *)&_sockListen.get_address(), (socklen_t *)&addrlen);
    if (_sockexchangeFd < 0)
    {
        std::cerr << "couldn't accept connection" << '\n';
        std::cerr << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void wsv::SimpleServer::print_read_msg()
{
    char _buffer[30000] = {0};
    read(_sockexchangeFd, _buffer, 30000);
    std::cout << _buffer << '\n';
}

int wsv::SimpleServer::get_fd() const
{
    return this->_sockexchangeFd;
}

void wsv::SimpleServer::write_msg(const char *msg)
{
    write(_sockexchangeFd, msg, strlen(msg));
}

wsv::SimpleServer::~SimpleServer()
{
}
