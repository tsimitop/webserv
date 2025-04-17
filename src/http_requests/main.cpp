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
		if (s.valid_inputs_ == NO)
			c.valid_config_ = NO;
	}
	if(c.valid_config_ == NO)
	{
		std::cerr << "Error: Non valid config!\n";
		return (1);
	}

	return (0);
}
