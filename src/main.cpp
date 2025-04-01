#include <cstdlib>
// #include "../inc/sockets/sockets.hpp"
#include "../inc/sockets/Server.hpp"
#include "../inc/http_requests/HttpRequest.hpp"

int	main(int argc, char** argv)
{
	HttpRequest		request;

	(void)argc;
	(void)argv;

	Server	server;
	server.run();

	// std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// start_server(); //start_server(request); write request in _HTTPRequest with setHttpRequest(std::string req);
	// request.readRequest(req);
	// if (!request.isValid())
	// {
	// 	std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
	// 	return (1);
	// }
	// request.extractPortFromHost(); //verify it's the expected behaviour
	// request.printRequest();
	// request.printHeaders();
	return EXIT_SUCCESS;
}

// GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n
/*
start_server();
instantiate Socket object (?)
pass buffer to HttpRequest object
read request
confirm validity
print for debug
*/
