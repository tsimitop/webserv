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
