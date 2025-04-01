#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/http_requests/HttpResponse.hpp"

int main(void)
{
	HttpRequest		request;
	HttpResponse	response;

	//store request body in the file that was mentioned in the disposition header
	/*
	POST /api/user HTTP/1.1
	Host: example.com
	Content-Disposition: attachment; filename="hello.txt"
	
	some text to be stored in a file->(hello.txt) // body is stored in
	*/
	// std::string	req = "GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "POST /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Length: 27\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nname=John+Doe&email=john@example.com\r\n";
	std::string	req = "POST /upload HTTP/1.1\r\nHost: localhost:8080\r\nContent-Type: application/octet-stream\r\nContent-Length: 12345\r\nContent-Disposition: attachment; filename='example.txt'\r\nConnection: keep-alive\r\n\r\n";
	// std::string	req = "DELETE /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n";
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
	catch(...)
	{
		std::cerr << RED << "Exception thrown: error while parsing http request.\n";
		exit(EXIT_FAILURE);
	}
	request.extractPortFromHost();
	if (request.getMethod() == "POST")
	{
		try
		{
			// TO BE ADDED AFTER CONFIG PARSING IS DONE!
			// int parse(std::void_t ServerConfig)
			// {
			// 	std::string uploadDir = ServerConfig->uploadDir || "./uploads";
			// 	std::string filename = getFilename();
			// if (!filename.empty())
			// 	uploadFile(uploadDir, filename);
			// }
			// request.uploadFile(request.getBasePath(), request.getFilename());
		}
		catch(...)
		{
			std::cerr << RED << "Exception thrown: error while uploading file.\n";
			exit(EXIT_FAILURE);
		}
		
	}
	request.printRequest();
	request.printHeaders();
	request.printBody();
	// std::string res = response.respond(request);
	// std::cout << YELLOW << res << QUIT << std::endl;
	return (0);
}

//____________________________________________________________________

// Name: HTTP Method = GET

// /posts/1 (Request Target / URL Path)
// Name: Request Target (or URI)

// HTTP/1.1 (Protocol Version)
// Name: HTTP Version