//______________________________________________________________________________
#include "../inc/http_requests/HttpRequest.hpp"
#include "../inc/http_requests/HttpResponse.hpp"
#include "../inc/config/Http.hpp"
#include "../inc/sockets_and_poll/Poll.hpp"


int main(int argc, char **argv)
{
	HttpResponse	response;
	std::cout << "STARTING WEB SERVER: you will be informed on the sockets listening soon.\n";
	// logToFile("entered main");
	//---------------------------Config parsing---------------------------
	Http c;
	if (argc > 2)
	{
		std::cerr << "Error: " <<  argv[0] << ": more than two arguments!\n";
		return 1;
	}
	c.preparingAndValidatingConfig(argc , argv);
	if (c.valid_config_ == NO)
	{
		std::cerr << "Error: Passing non existing | wrong config | config with typos!\n";
		return 1;
	}
	c.parsingServers();
	// we need to check if all the necessayry atributes are serred localhost, max bodylen and timeout
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
