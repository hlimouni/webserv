#include "Socket.hpp"

WSV::Socket::Socket()
{
}

WSV::Socket::Socket(int domain, int type, int port)
{
    this->_fd = socket(domain, type, 0);
    if (this->_fd == 0)
    {
        std::cerr << "Coundn't open socket" << '\n';
        exit(EXIT_FAILURE);
    }
    this->_addrlen = sizeof(_address);
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(port);
    memset(_address.sin_zero, '\0', sizeof (_address.sin_zero));
}

WSV::Socket::Socket(WSV::Socket const & copy)
{
    *this = copy;
}

WSV::Socket const & WSV::Socket::operator=(WSV::Socket const & rhs)
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

void WSV::Socket::bind_name()
{
    if (bind(_fd, (struct sockaddr *)&_address, _addrlen) < 0)
    {
        std::cerr << "Coundn't bind the socket" << '\n';
        exit(EXIT_FAILURE);
    }
}

void WSV::Socket::init_connection()
{
    if (connect(_fd, (struct sockaddr *)&_address, _addrlen) < 0)
    {
        std::cerr << "Coundn't connect the socket" << '\n';
        exit(EXIT_FAILURE);
    }

}

int WSV::Socket::get_fd() const
{
    return this->_fd;
}

struct sockaddr_in const & WSV::Socket::get_address()
{
    return this->_address;
}

size_t WSV::Socket::len() const
{
    return this->_addrlen;
}

WSV::Socket::~Socket()
{
}
