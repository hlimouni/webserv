#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdexcept>

namespace wsv
{
	class Socket
	{
	
	private:
		int _fd;
		struct sockaddr_in _address;
		size_t _addrlen;

	public:
		Socket();
		Socket(int domain, int type, int port);
		Socket(Socket const &);
		Socket const & operator=(Socket const & rhs);
		~Socket();

		void bind_name();
		void init_connection();
		void listen_to_port(int backlog);
		int  get_fd() const;
		size_t len() const;
		struct sockaddr_in const & get_address() const;
	};
}

#endif