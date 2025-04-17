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
	
// // POST
// 	std::string	req = "POST /uploads HTTP/1.1\r\n\
// Host: localhost:4242\r\n\
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:135.0) Gecko/20100101 Firefox/135.0\r\n\
// Accept: */*\r\n\
// Accept-Language: en-US,en;q=0.5\r\n\
// Accept-Encoding: gzip, deflate, br, zstd\r\n\
// Referer: http://localhost:8080/\r\nContent-Type: application/octet-stream\r\n\
// Content-Disposition: attachment; filename='test.txt'\r\nContent-Length: 1\r\n\
// Origin: http://localhost:8080\r\nConnection: keep-alive\r\nSec-Fetch-Dest: empty\r\nSec-Fetch-Mode: cors\r\n\
// Sec-Fetch-Site: same-origin\r\nPriority: u=0\r\n\r\nH";

// DELETE
// std::string	req = "DELETE /uploads/test.txt HTTP/1.1\r\n\
// Host: localhost:8080\r\n\
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:135.0) Gecko/20100101 Firefox/135.0\r\n\
// Accept: */*\r\n\
// Accept-Language: en-US,en;q=0.5\r\n\
// Accept-Encoding: gzip, deflate, br, zstd\r\n\
// Referer: http://localhost:8080/\r\n\
// Origin: http://localhost:8080\r\n\
// Connection: keep-alive\r\n\
// Sec-Fetch-Dest: empty\r\n\
// Sec-Fetch-Mode: cors\r\n\
// Sec-Fetch-Site: same-origin\r\n\
// Priority: u=0\r\n\r\n";

	// std::string	req = "GET /indexx.html HTTP/1.1\r\nHost: www.example.com:4242\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "DELETE /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::stringstream line(req);
	// std::string		word;
	// line >> word;
	// ServerInfo current_server = c.servers_[0];
	// std::vector<std::string> allowed_methods = c.servers_[0].locations_[0].allowed_methods_;
	// //----------------Request has come in from client----------------//
	// HttpRequest		request(req, current_server);
	// try
	// {
	// 	std::cout << CYAN << "Request: " << req << QUIT << std::endl;
	// 	request.readRequest(req);
	// 	if (!request.isValid())
	// 	{
	// 		std::cout << RED << "Error: Invalid Request" << QUIT << std::endl;
	// 		throw HttpRequest::httpParserException();
	// 	}
	// }
	// catch(...)
	// {
	// 	std::cerr << RED << "Exception thrown: error while parsing http request.\n";
	// 	exit(EXIT_FAILURE);
	// }
	// request.extractPortFromHost();
	// //----------------Request validated, will be executed----------------//
	// response = request.performMethod();

	// // request.printRequest();
	// // request.printHeaders();
	// // request.printBody();

	// //----------------Response should be generated----------------//
	// std::string res = response.respond(request);
	// std::cout << YELLOW << res << QUIT << std::endl;
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