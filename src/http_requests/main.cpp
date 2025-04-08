#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/http_requests/HttpResponse.hpp"
#include "../../inc/config/Http.hpp"

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

	// for (ServerInfo s : c.servers_)
	// {
	// 	if (s.valid_inputs_ == NO)
	// 	{
	// 		std::cerr << "Error: Non valid config!\n";
	// 		return (1);
	// 	}
	// }
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
	//----------------------End of config parsing--------------------------
	
	std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:4242\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 27\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com\r\n";
	// std::string	req = "POST /upload HTTP/1.1\r\nHost: localhost:8080\r\nContent-Type: application/octet-stream\r\nContent-Length: 41\r\nContent-Disposition: attachment; filename='/Users/tsimitop/Documents/42_coding/webserv_workspace/webserv/attempt'\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "DELETE /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	std::stringstream line(req);
	std::string		word;
	line >> word;
	ServerInfo current_server = c.servers_[0];
	std::vector<std::string> allowed_methods = c.servers_[0].locations_[0].allowed_methods_;
	if ( std::find(allowed_methods.begin(), allowed_methods.end(), word) != allowed_methods.end())
	std::cout << word << " found!\n";
	//----------------Request has come in from client----------------//
	HttpRequest		request(req, current_server);
	try
	{
		request.readRequest(req);
		std::cout << CYAN << "Request: " << req << QUIT << std::endl;
		if (!request.isValid())
		{
			std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
			throw HttpRequest::httpParserException();
		}
	}
	catch(...)
	{
		std::cerr << RED << "Exception thrown: error while parsing http request.\n";
		exit(EXIT_FAILURE);
	}
	request.extractPortFromHost();
	if (current_server.listen_ == request.getPort())
		std::cout << "Port validated!\n";
	//----------------Request validated, will be executed----------------//

	response = request.performMethod();

	// request.printRequest();
	// request.printHeaders();
	// request.printBody();

	//----------------Response should be generated----------------//
	std::string res = response.respond(request);
	std::cout << YELLOW << res << QUIT << std::endl;
	return (0);
}
//_________________
// What Real Web Servers Do (p. 113, squirrel book)
// 1. Set up connection	—accept a client connection, or close if the client is unwanted.
// 2. Receive request	—read an HTTP request message from the network.
// 3. Process request	—interpret the request message and take action.
// 4. Access resource	—access the resource specified in the message.
// 5. Construct response—create the HTTP response message with the right headers.
// 6. Send response		—send the response back to the client.
// 7. Log transaction	—place notes about the completed transaction in a log file.

//____________________________________________________________________

// Name: HTTP Method = GET

// /posts/1 (Request Target / URL Path)
// Name: Request Target (or URI)

// HTTP/1.1 (Protocol Version)
// Name: HTTP Version