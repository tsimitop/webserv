// #include <cstdlib>
// // #include "../inc/sockets/sockets.hpp"
// #include "../inc/sockets/Server.hpp"
// #include "../inc/http_requests/HttpRequest.hpp"
// #include "../inc/config/Http.hpp"

// int	main(int argc, char** argv)
// {
// 	HttpRequest		request;

// 	Http c;
// 	if (argc > 2)
// 	{
// 		std::cerr << "Error: " <<  argv[0] << ": more than two arguments!\n";
// 		return 1;
// 	}
// 	c.preparingAndValidatingConfig(argc , argv);
// 	if(c.valid_config_ == NO)
// 	{
// 		std::cerr << "Error: Non valid config!\n";
// 		return (1);
// 	}
// 	c.parsingServers();

// 	Server	server;
// 	server.run();

// 	return EXIT_SUCCESS;
// }

//______________________________________________________________________________
#include "../inc/http_requests/HttpRequest.hpp"
#include "../inc/http_requests/HttpResponse.hpp"
#include "../inc/config/Http.hpp"
#include "../inc/sockets_and_poll/Poll.hpp"

int main(int argc, char **argv)
{
	HttpResponse	response;
	
	//---------------------------Config parsing---------------------------
	Http c;
	if (argc > 2)
	{
		std::cerr << "Error: " <<  argv[0] << ": more than two arguments!\n";
		return 1;
	}
	c.preparingAndValidatingConfig(argc , argv);

	c.parsingServers();
	for (ServerInfo s : c.servers_)
	{
		if (s.valid_inputs_ == NO)
			c.valid_config_ = NO;
	}
	if(c.valid_config_ == NO)
	{
		std::cerr << "Error: Non valid config!\n";
		return (1);
	}
	Poll poll_one;
	poll_one.setConfig(c);
	if (poll_one.binding() == YES)
		poll_one.synchroIO();
	return (0);
}
