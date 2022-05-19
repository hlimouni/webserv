#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <algorithm>

#define PORT 9909


struct sockaddr_in srv;

int main()
{
	//Initialize the socket
	int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nSocket < 0)
	{
		std::cout << "Socket not opened";
	}
	else
	{
		std::cout << "Socket opened successfully with id " << nSocket;
	}
	std::cout << std::endl;

	//Initilize the environment for sockaddr structure
	srv.sin_family = AF_INET;
	//htons (host to network short 16bit)
	srv.sin_port = htons(PORT);
	srv.sin_addr.s_addr = INADDR_ANY;
	bzero(&srv.sin_zero, 8);

	//bind the socket to the local port
	int nRet = bind(nSocket, reinterpret_cast<sockaddr*>(&srv), sizeof(sockaddr));
	if (nRet < 0)
	{
		std::cout << "Failed to";
	}


}