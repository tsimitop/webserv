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

static bool isOnlyWhitespace(const std::string& str)
{
	for (char c : str)
	{
		if (!std::isspace(static_cast<unsigned char>(c)))
			return false;
	}
	return true;
}

static bool isOnlyDigit(const std::string& str)
{
	int i = 0;
	for (char c : str)
	{
		if (i == 0)
		{
			if (std::isspace(static_cast<unsigned char>(c)))
			continue;
		}
		if (!std::isdigit(static_cast<unsigned char>(c)))
			return false;
		i++;
	}
	return true;
}

// bool	HttpRequest::validateDelete(void)
// {
	// to be coded only if Authorization is found to be required or we implement cookies
// }

// RFC 9110: A user agent SHOULD NOT send a Content-Length header field when the request
// message does not contain content and the method semantics do not anticipate such data.
// https://httpwg.org/specs/rfc9110.html#field.content-length
bool	HttpRequest::validatePost(void)
{
	auto it = _headers.begin();
	int	contType = 0;
	int	contLength = 0;

	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		if (it->first == "Content-Type")
		{
			// std::cout << "'" << it->first << "' " << "'" << it->second << "'\n";
			if (it->second.empty() || isOnlyWhitespace(it->second)) // else make sure it's acceptable input
			{
				std::cout << RED << "Content-Type isn't specified" << QUIT << std::endl;
				return (false);
			}
			contType++;
		}
		if (it->first == "Content-Length") // else make sure it's acceptable input
		{
			if (it->second.empty() || isOnlyWhitespace(it->second))
			{
				std::cout << RED << "Content-Length isn't specified" << QUIT << std::endl;
				return (false);
			}
			try
			{
				if (stoi(it->second) < 0 || !isOnlyDigit(it->second))
				{
					std::cout << RED << "Content-Length is not acceptable: " << it->second << QUIT << std::endl;
					return (false);
				}
			}
			catch (const std::invalid_argument)
			{
				std::cout << RED << "Content-Length specifier is not an int" << QUIT << std::endl;
				return (false);
			}
			contLength++;
		}
	}
	if (contType == 1 && contLength == 1)
		return (true);
	return (false);
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
	if (_method == "POST")
	{
		if (!validatePost())
		{
			std::cout << RED << "Post could not be validated" << QUIT << std::endl;
			return (1);
		}
	}
	// if (_method == "DELETE") //NOT YET I THINK
	// 	if (!validateDelete())
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
	std::cout << "method:\t\t"<< _method << std::endl;
	std::cout << "url:\t\t"<< _url << std::endl;
	std::cout << "version:\t"<< _version << std::endl;
	std::cout << "PORT:\t\t"<< _port << QUIT << std::endl;
	std::cout << std::endl;
}

void	HttpRequest::printHeaders(void) const
{
	std::cout << CYAN << "Printing headers: " << std::endl;
	for (const auto& pair : _headers)
		std::cout << pair.first << ":\t" << pair.second << std::endl;;
	std::cout << QUIT << std::endl;
}


/*
Mandatory Headers
GET
Host: example.com

POST
Host: example.com
Content-Type: /text/plain
Content-Length: 15

also request body if needed ie
{
  "name": "John Doe",
  "email": "john@example.com"
}


DELETE
Host: example.com
Authorization: Bearer <token>  (if required)
Accept: application/json  (if required)
*/