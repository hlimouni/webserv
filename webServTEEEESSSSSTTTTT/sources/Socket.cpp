#include "../headers/Socket.hpp"

wsv::Socket::Socket() : _fd(-1), _addrlen(0)
{
}

wsv::Socket::Socket(int domain, int type, int port)
{
    this->_fd = socket(domain, type, 0);
    if (this->_fd < 0)
        throw std::runtime_error("Coundn't open socket");
    if (fcntl(this->_fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Coundn't make socket non-blocking");
    this->_addrlen = sizeof(_address);
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(port);
    memset(_address.sin_zero, '\0', sizeof (_address.sin_zero));
}

wsv::Socket::Socket(wsv::Socket const & copy)
{
    *this = copy;
}

wsv::Socket const & wsv::Socket::operator=(wsv::Socket const & rhs)
{
    if (this != &rhs)
    {
        this->_fd = rhs._fd;

        // this->_address.sin_addr.s_addr = rhs._address.sin_addr.s_addr;
        // this->_address.sin_family = rhs._address.sin_family;
        // this->_address.sin_len = rhs._address.sin_len;
        // this->_address.sin_port = rhs._address.sin_port;
        // memmove(&this->_address.sin_zero, &rhs._address.sin_zero, 8);

        memmove(&_address, &rhs._address, sizeof(_address));

        this->_addrlen = rhs._addrlen;
    }
    return *this;
}

void wsv::Socket::bind_name()
{
    if (bind(_fd, (struct sockaddr *)&_address, _addrlen) < 0)
    {
        throw std::runtime_error("Coundn't bind the socket");
    }
}

void wsv::Socket::init_connection()
{
    if (connect(_fd, (struct sockaddr *)&_address, _addrlen) < 0)
    {
        throw std::runtime_error("Coundn't connect the socket");
    }

}

void wsv::Socket::listen_to_port(int backlog)
{
    if (listen(this->_fd, backlog) < 0)
    {
        throw std::runtime_error("Socket failed to listen to port");
    }

}

int wsv::Socket::get_fd() const
{
    return this->_fd;
}

void wsv::Socket::set_fd(int fd)
{
    this->_fd = fd;
}

void wsv::Socket::set_address(sockaddr_in const & addr)
{
    this->_address = addr;
}

void wsv::Socket::set_len(size_t len)
{
    this->_addrlen = len;
}

struct sockaddr_in const & wsv::Socket::get_address() const
{
    return this->_address;
}

size_t wsv::Socket::len() const
{
    return this->_addrlen;
}

wsv::Socket::~Socket()
{
}
