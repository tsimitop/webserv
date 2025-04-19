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
	c.parsingServers();
	for (ServerInfo s : c.servers_)
	{
		if (s.valid_server_ == NO)
			c.valid_config_ = NO;
	}
	if(c.valid_config_ == NO)
	{
		std::cerr << "Error: Non valid config!\n";
		return (1);
	}
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