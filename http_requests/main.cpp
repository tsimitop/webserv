#include <iostream>
#include <string>
#include "HttpRequest.hpp"

std::string removeFirstWord(const std::string &input)
{
	std::string::size_type firstSpace = input.find(' ');
	if (firstSpace == std::string::npos)
		return "";
	return (input.substr(firstSpace + 1));
}

std::string *parseRequest(std::string &httpRequest)
{
	std::string *parsedRequest = new std::string[3];
	std::string get = "GET";
	int		 compared;

	compared = httpRequest.compare(0, 3, get);
	if (compared == 0)
	{
		std::cout << "Request method is GET" << std::endl;
		parsedRequest[0] = "GET";
	}
	else
		parsedRequest[0] = "UNKNOWN";
	httpRequest = removeFirstWord(httpRequest);

	std::string::size_type firstSpace = httpRequest.find(' ');
	std::string url;
	if (firstSpace == std::string::npos)
	{
		std::cout << "no second word\n";
		parsedRequest[1] = "";
	}
	else
	{
		url = httpRequest.substr(0, firstSpace);
		parsedRequest[1] = url;
	}
	httpRequest = removeFirstWord(httpRequest);
	firstSpace = httpRequest.find(' ');
	std::string version;
	version = httpRequest.substr(0, firstSpace);
	parsedRequest[2] = version;
	return (parsedRequest);
}

int main(int argc, char **argv)
{
	HttpRequest		request;
	std::ifstream	infile(argv[1]);

	if (argc != 2)
	{
		std::cout << "Wrong input\n";
		return (1);
	}
	if (!infile)
	{
		std::cout << "unable to open file\n";
		return (1);
	}

	request.readRequest(infile);
	infile.close();
// std::cout << request.getMethod() << std::endl;
	request.printHeaders();
	// std::string toParse = "GET /home/page HTTP/1.1\r\nHost: ramy.com\r\nContent-Length: 90\r\n";
	return (0);
}

//____________________________________________________________________
// #include <iostream>
// #include <string>


// std::string removeFirstWord(const std::string &input)
// {
// 	std::string::size_type firstSpace = input.find(' ');
// 	if (firstSpace == std::string::npos)
// 		return "";
// 	return (input.substr(firstSpace + 1));
// }

// std::string *parseRequest(std::string &httpRequest)
// {
// 	std::string *parsedRequest = new std::string[3];
// 	std::string get = "GET";
// 	int		 compared;

// 	compared = httpRequest.compare(0, 3, get);
// 	if (compared == 0)
// 	{
// 		std::cout << "Request method is GET" << std::endl;
// 		parsedRequest[0] = "GET";
// 	}
// 	else
// 		parsedRequest[0] = "UNKNOWN";
// 	httpRequest = removeFirstWord(httpRequest);

// 	std::string::size_type firstSpace = httpRequest.find(' ');
// 	std::string url;
// 	if (firstSpace == std::string::npos)
// 	{
// 		std::cout << "no second word\n";
// 		parsedRequest[1] = "";
// 	}
// 	else
// 	{
// 		url = httpRequest.substr(0, firstSpace);
// 		parsedRequest[1] = url;
// 	}
// 	httpRequest = removeFirstWord(httpRequest);
// 	firstSpace = httpRequest.find(' ');
// 	std::string version;
// 	version = httpRequest.substr(0, firstSpace);
// 	parsedRequest[2] = version;
// 	return (parsedRequest);
// }

// int main(void)
// {
// 	std::string toParse = "GET /home/page HTTP/1.1";
// 	std::string *parsedRequest = parseRequest(toParse);
// 	std::cout << parsedRequest[0] << std::endl;
// 	std::cout << parsedRequest[1] << std::endl;
// 	std::cout << parsedRequest[2] << std::endl;
// 	delete [] parsedRequest;
// 	return (0);
// }

// Name: HTTP Method = GET

// /posts/1 (Request Target / URL Path)
// Name: Request Target (or URI)

// HTTP/1.1 (Protocol Version)
// Name: HTTP Version

//c++ -g -Wall -Werror -Wextra -readline -fsanitize=address -std=c++17 main.cpp