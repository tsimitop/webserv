#include "HttpRequest.hpp"

// Orthodox Canonical Class Form
HttpRequest::HttpRequest() : _httpRequest(""), _method(""), _url(""), _version("") {}

HttpRequest::HttpRequest(const HttpRequest& other) {*this = other;}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->_headers = other._headers;
		this->_httpRequest = other._httpRequest;
		this->_method = other._method;
		this->_url = other._url;
		this->_version = other._version;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {}

// Parameterized constructor
HttpRequest::HttpRequest(std::string& request) : _headers(0), _httpRequest(request), _method(nullptr), _url(nullptr), _version(nullptr) {}

// Getters
std::unordered_map<std::string, std::string> HttpRequest::getHeaders(void) const
{return (_headers);}

std::string HttpRequest::getHttpRequest(void) const
{return (_httpRequest);}

std::string HttpRequest::getMethod(void) const
{return (_method);}

std::string HttpRequest::getUrl(void) const
{return (_url);}

std::string HttpRequest::getVersion(void) const
{return (_version);}

int HttpRequest::getPort(void) const
{return (_port);}

// Setters
void HttpRequest::setHttpRequest(std::string req)
{_httpRequest = req;}

void HttpRequest::setMethod(std::string meth)
{_method = meth;}

void HttpRequest::setUrl(std::string url)
{_url = url;}

void HttpRequest::setVersion(std::string ver)
{_version = ver;}

// Parse
static std::string removeFirstWord(const std::string &input)
{
	std::string::size_type firstSpace = input.find(' ');
	if (firstSpace == std::string::npos)
		return (input);
	return (input.substr(firstSpace + 1, std::string::npos));
}

static bool	isValidMethod(std::string method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
		return (true);
	return (false);
}

void	HttpRequest::parseRequestLine(std::string& line)
{
	std::string::size_type	firstSpace;
	std::string				method;
	std::string				url;
	std::string				version;

// Request method
	_httpRequest = line;
	firstSpace = line.find(' ');
	method = line.substr(0, firstSpace);
	if (isValidMethod(method))
		_method = method;
	else
		_method = "UKNOWN";
	
// Url
	line = removeFirstWord(line);
	firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
	{
		std::cout << "no url provided\n";
		_url = "";
	}
	else
		_url = line.substr(0, firstSpace);

// HTTP version
	version = removeFirstWord(line);
	if (version.size() == 0)
		std::cout << "no version provided";
	else
		_version = version;

}

void	HttpRequest::parseLine(std::string line)
{
	std::string::size_type	colonPos;
	colonPos = line.find(':');
	if (colonPos != std::string::npos)
	{
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1, std::string::npos);
		_headers.insert(std::pair<std::string, std::string>(key, value));
	}
	else
	{
		std::cout << RED << "No colon after key, no idea what to do" << QUIT << std::endl;
		return ;
	}
}

void	HttpRequest::readRequest(std::string requestLine)
{
	std::string	line = requestLine.substr(0, requestLine.find("\r\n"));
	if (!line.empty())
		parseRequestLine(line);

	requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
// std::cout << "requestLine: " << requestLine << std::endl;
	line = requestLine.substr(0, requestLine.find("\r\n"));
// std::cout << "line: " << line << std::endl;
	while (!line.empty())
	{
		parseLine(line);
		requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
// std::cout << "requestLine: " << requestLine << std::endl;
		line = requestLine.substr(0, requestLine.find("\r\n"));
// std::cout << "line: " << line << std::endl;
	}
}

bool	HttpRequest::isValid()
{
	if (_method == "UKNOWN" || _url.empty() || _version.empty())
	{
		if (_method == "UKNOWN")
			std::cout << RED << "Can't handle uknown method->400 Bad Request" << QUIT << std::endl;
		if (_url.empty())
			std::cout << RED << "No url->server should be closed by foreign host" << QUIT << std::endl;
		if (_version.empty())
			std::cout << RED << "No HTTP version->server should be close by foreign host" << QUIT << std::endl;
		return (false);
	}
	auto it = _headers.begin();
	for (it = _headers.begin(); it != _headers.end(); it++)
		if (it->first == "Host")
			break;
	if (it == _headers.end())
	{
		std::cout << RED << "Didn't find Host" << QUIT << std::endl;
		return (false);
	}
	return (true);
}

void	HttpRequest::extractPortFromHost()
{
	auto	it = _headers.begin();
	for (it = _headers.begin(); it != _headers.end(); it++)
		if (it->first == "Host")
			break;

	std::string	host = it->second;
	auto		pos = host.find(':');
	if (pos == std::string::npos)
	{
		_port = 80;
		return ;
	}
	std::string	port = it->second.substr(pos + 1, std::string::npos);
	int			portNbr;
	try {
		portNbr = stoi(port);
	}
	catch (const std::invalid_argument) {
		std::cout << RED << "Exception thrown: Couldn't convert port to int" << QUIT << std::endl;
		return;
	}

	if (portNbr > 0 && portNbr < 65536) // NOT SURE ABOUT THIS CHECK!!!
		_port = portNbr;
	else
		std::cout << RED << "Consider choosing another port instead of PORT:" << portNbr << QUIT << std::endl;
}

// Debug
void	HttpRequest::printRequest(void) const
{
	std::cout << MAGENTA << "Printing request: " << std::endl;
    std::cout << _httpRequest << std::endl;
    std::cout << "method: "<< _method << std::endl;
    std::cout << "url: "<< _url << std::endl;
    std::cout << "version: "<< _version << std::endl;
    std::cout << "PORT: "<< _port << QUIT << std::endl;
	std::cout << std::endl;
}

void	HttpRequest::printHeaders(void) const
{
	std::cout << CYAN << "Printing headers: " << std::endl;
    for (const auto& pair : _headers)
        std::cout << pair.first << ": " << pair.second << std::endl;;
	std::cout << QUIT << std::endl;
}
