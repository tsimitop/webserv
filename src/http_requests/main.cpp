#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/http_requests/HttpResponse.hpp"

int main(int argc, char **argv)
{
	HttpRequest		request;
	HttpResponse	response(208);

	std::cout<< "\nStatus Code: " << response.getStatusCode() << ", Reason Phrase: " << response.getReasonPhrase() << std::endl << std::endl;

	(void)argc;
	(void)argv;

	// std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "DELETE /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "POST /api/user HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/json\r\nContent-Length: 33\r\nAuthorization: Bearer <your_token>\r\n\r\n";
	// std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 27\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com\r\n";
	std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 75\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com&name=Mary+Doe&email=mary@example.com\r\n";
	request.readRequest(req);
	if (!request.isValid())
	{
		std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
		return (1);
	}
	request.extractPortFromHost(); //verify it's the expected behaviour
	request.printRequest();
	request.printHeaders();
	request.printBody();
	return (0);
}

//____________________________________________________________________

// Name: HTTP Method = GET

// /posts/1 (Request Target / URL Path)
// Name: Request Target (or URI)

// HTTP/1.1 (Protocol Version)
// Name: HTTP Version

//c++ -g -Wall -Werror -Wextra -readline -fsanitize=address -std=c++17 main.cpp