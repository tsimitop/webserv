#include "HttpRequest.hpp"

int main(int argc, char **argv)
{
	HttpRequest		request;

	(void)argc;
	(void)argv;

	std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	request.readRequest(req);
	request.extractPortFromHost();
	request.printRequest();
	request.printHeaders();
	if (!request.isValid())
	{
		std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
		return (1);
	}
	return (0);
}

//____________________________________________________________________

// Name: HTTP Method = GET

// /posts/1 (Request Target / URL Path)
// Name: Request Target (or URI)

// HTTP/1.1 (Protocol Version)
// Name: HTTP Version

//c++ -g -Wall -Werror -Wextra -readline -fsanitize=address -std=c++17 main.cpp