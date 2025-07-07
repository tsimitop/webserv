//______________________________________________________________________________
#include "../inc/http_requests/HttpRequest.hpp"
#include "../inc/http_requests/HttpResponse.hpp"
#include "../inc/config/Http.hpp"
#include "../inc/sockets_and_poll/Poll.hpp"

int main(int argc, char **argv)
{
	// #if defined(__APPLE__)
	// {
	// 	std::cout << RED << "Please compile in Docker ❤️❤️❤️\n" << QUIT;
	// 	std::cout << BLUE << "Use: make docker_run\n" << QUIT;
	// 	return (0);
	// }
	// #endif
	signal(SIGINT, signalHandler);
	signal(SIGPIPE, signalHandlerTwo);
	HttpResponse	response;
	std::cout << YELLOW <<  "STARTING WEB SERVER: you will be informed on the sockets listening soon.\n" << QUIT;
	Http c;
	if (argc > 2)
	{
		std::cerr << "Error: " <<  argv[0] << ": more than two arguments!\n";
		return 1;
	}
	c.preparingAndValidatingConfig(argc , argv);
	if (c.valid_config_ == NO)
		return 1;
	c.parsingServers();
	c.validPostParsing();

	if(c.valid_config_ == NO)
	{
		std::cerr << "Error: You need atleast one valid server!\n";
		return (1);
	}
	Poll poll_one;
	poll_one.setConfig(c);
	if (poll_one.binding() != NO)
		poll_one.synchroIO();
	return (0);
}
/*
Notes for evaluator:
brew install siege
siege -b 0.0.0.0:4242
siege -b localhost:4242
curl --resolve example.com:4242:127.0.0.1 http://example.com/
curl --resolve example.com:4245:127.0.0.1 http://example.com/


curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Disposition: attachment; filename=testing" --data "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feug" http://localhost:4246/www2/uploads2
*/

/*
UKNOWN /uploads HTTP/1.1
__________________________________________
POST /uploads HTTP/1.1
Host: localhost:4242
Connection: keep-alive
Content-Length: 7
Content-Disposition: attachment; filename="hello.md"
Content-Type: application/octet-stream

hello

__________________________________________

DELETE /uploads2%2Fup.md HTTP/1.1
Host: localhost:4242
Connection: close

__________________________________________

uploads2/up5.md
DELETE /uploads2%2Fup5.md HTTP/1.1
Host: localhost:4242
__________________________________________

// */
