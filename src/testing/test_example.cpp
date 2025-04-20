
#include "../../inc/testing/HttpRequestTester.hpp"

std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 27\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com\r\n";

try
{
	request.readRequest(req);
	if (!request.isValid())
	{
		std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
		throw HttpRequest::httpParserException();
		// return (1); // replace with exception maybe
	}
}
// catch(const std::exception& e)
catch(...)
{
	// std::cerr << RED << e.what() << std::endl;;
	std::cerr << RED << "Exception thrown: error while parsing http request.\n";
	exit(EXIT_FAILURE);
}
request.extractPortFromHost(); //verify it's the expected behaviour
request.printRequest();
request.printHeaders();
// request.printBody();
std::string res = response.respond(request);
std::cout << YELLOW << res << QUIT << std::endl;
return (0);


int main(int argc, char **argv)
{
	HttpRequest		request;
	HttpResponse	response;
	// HttpResponse	response(208);

	// std::cout<< "\nStatus Code: " << response.getStatusCode() << ", Reason Phrase: " << response.getReasonPhrase() << std::endl << std::endl;

	(void)argc;
	(void)argv;
	
	//PUT THEM IN MAP FOR TESTING
	//store request body in the file that was mentioned in the disposition header
	/*
	POST /api/user HTTP/1.1
	Host: example.com
	Content-Disposition: attachment; filename="hello.txt"
	
	some text to be stored in a file->(hello.txt) // body is stored in file
	*/
	// std::string	req = "GET /index.html HTTP/1.1\r\n\r\n\r\n";
	// std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "DELETE /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "POST /api/user HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/json\r\nContent-Length: 33\r\nAuthorization: Bearer <your_token>\r\n\r\n";
	std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 27\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com\r\n";
	// std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 75\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com&name=Mary+Doe&email=mary@example.com\r\n";
	try
	{
		request.readRequest(req);
		if (!request.isValid())
		{
			std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
			throw HttpRequest::httpParserException();
			// return (1); // replace with exception maybe
		}
	}
	// catch(const std::exception& e)
	catch(...)
	{
		// std::cerr << RED << e.what() << std::endl;;
		std::cerr << RED << "Exception thrown: error while parsing http request.\n";
		exit(EXIT_FAILURE);
	}
	request.extractPortFromHost(); //verify it's the expected behaviour
	request.printRequest();
	request.printHeaders();
	request.printBody();
	std::string res = response.respond(request);
	std::cout << YELLOW << res << QUIT << std::endl;
	return (0);
}
