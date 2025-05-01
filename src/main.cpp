//______________________________________________________________________________
#include "../inc/http_requests/HttpRequest.hpp"
#include "../inc/http_requests/HttpResponse.hpp"
#include "../inc/config/Http.hpp"
#include "../inc/sockets_and_poll/Poll.hpp"

int main(int argc, char **argv)
{
	signal(SIGINT, signalHandler);
	// signal(SIGPIPE, signalHandler);
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
		std::cerr << "Error: You need atleast one valid server!\n"; //
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
siege -b localhost:4242
curl --resolve example.com:4242:127.0.0.1 http://example.com/
curl --resolve example.com:4245:127.0.0.1 http://example.com/


curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Disposition: attachment; filename=testing" --data "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feug" http://localhost:4242/www2/uploads2
curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Disposition: attachment; filename=testing" --data-binary @/Users/tsimitop/Desktop/test.txt http://localhost:4242/www2/uploads2
*/

/*
POST /uploads HTTP/1.1
UKNOWN /uploads HTTP/1.1
Host: localhost:4242
Connection: keep-alive
Content-Length: 0
Content-Disposition: attachment; filename="hello.md"
Content-Type: application/octet-stream

hello

DELETE /uploads2%2Fup.md HTTP/1.1
Host: localhost:4242
Connection: close

POST /uploads HTTP/1.1
Host: localhost:4242
Connection: keep-alive
Content-Length: 297
sec-ch-ua: "Chromium";v="128", "Not;A=Brand";v="24", "Google Chrome";v="128"
sec-ch-ua-platform: "macOS"
DNT: 1
Content-Disposition: attachment; filename="up3.md"
sec-ch-ua-mobile: ?0
Content-Type: application/octet-stream
Accept: */*
Origin: http://localhost:4242
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: cors
Sec-Fetch-Dest: empty
Referer: http://localhost:4242/index.html
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: en-US,en;q=0.9



Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1
Host: localhostET / HTTP/1.1klnkkl
*/