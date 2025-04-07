#include <cstdlib>
// #include "../inc/sockets/sockets.hpp"
#include "../inc/sockets/Server.hpp"
#include "../inc/http_requests/HttpRequest.hpp"
#include "../inc/config/Http.hpp"

int	main(int argc, char** argv)
{
	HttpRequest		request;

	Http c;
	if (argc > 2)
	{
		std::cerr << "Error: " <<  argv[0] << ": more than two arguments!\n";
		return 1;
	}
	c.preparingAndValidatingConfig(argc , argv);
	if(c.valid_config_ == NO)
	{
		std::cerr << "Error: Non valid config!\n";
		return (1);
	}
	c.parsingServers();

	Server	server;
	server.run();

	return EXIT_SUCCESS;
}
