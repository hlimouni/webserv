
#include "../headers/WebServer.hpp"

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		try
		{
			configParser config(argv[1]);
			WebServer	ws(config);
		}
		catch(const std::exception& e)
		{
			std::cerr << RED << e.what() << '\n';
		}
	}
	else
		std::cout << "Error, you should pass a config file" << std::endl;
	return (0);
}