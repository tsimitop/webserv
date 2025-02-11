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

// Setters
void HttpRequest::setHttpRequest(std::string req)
{_httpRequest = req;}

void HttpRequest::setMethod(std::string meth)
{_method = meth;}

void HttpRequest::setUrl(std::string url)
{_url = url;}

void HttpRequest::setVersion(std::string ver)
{_version = ver;}

static std::string removeFirstWord(const std::string &input)
{
	std::string::size_type firstSpace = input.find(' ');
	if (firstSpace == std::string::npos)
		return "";
	return (input.substr(firstSpace + 1));
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
	
	if (line.find("\\r") == std::string::npos)
	{
		std::cout << "Didn't find \\r\\n in : " << line << std::endl;
		return ;
	}
	if (line.back() != '\r')
	{
		std::cout << "\\r not in the write position in : " << line << std::endl;
		return ;
	}

	_httpRequest = line;
	firstSpace = line.find(' ');
	method = _httpRequest.substr(0, firstSpace);
	if (isValidMethod(method))
		_method = method;
	else
		_method = "UKNOWN";
	
	removeFirstWord(line);
	firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
	{
		std::cout << "no url provided\n";
		_url = "";
	}
	else
		_url = _httpRequest.substr(0, firstSpace);

	version = _httpRequest.substr(0, line.find("\\r"));
	if (version.size() == 0)
	{
		std::cout << "no version provided";
		_version = version;
	}
}

void	HttpRequest::parseLine(std::string line)
{
	(void)line;
}

void	HttpRequest::readRequest(std::ifstream& requestFile)
{
	std::string	line;

	if (std::getline(requestFile, line) && !line.empty())
		parseRequestLine(line);
	while (std::getline(requestFile, line) && !line.empty())
	{
		parseLine(line);
	}
}

void	HttpRequest::printHeaders(void) const
{
	std::cout << "Printing request: " << std::endl;
    for (const auto& pair : _headers)
        std::cout << pair.first << ": " << pair.second << std::endl;
}
