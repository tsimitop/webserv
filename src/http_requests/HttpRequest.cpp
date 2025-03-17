#include "../../inc/http_requests/HttpRequest.hpp"

// Orthodox Canonical Class Form
HttpRequest::HttpRequest() : bodyComplete_(""), httpRequest_(""), method_(""), url_(""), version_(""), port_(80) {}

HttpRequest::HttpRequest(const HttpRequest& other) {*this = other;}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->headers_ = other.headers_;
		this->bodyVector_ = other.bodyVector_;
		this->bodyComplete_ = other.bodyComplete_;
		this->httpRequest_ = other.httpRequest_;
		this->method_ = other.method_;
		this->url_ = other.url_;
		this->version_ = other.version_;
		this->port_ = other.port_;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {}

// Parameterized constructor
HttpRequest::HttpRequest(std::string& request) : headers_(0), httpRequest_(request), method_(nullptr), url_(nullptr), version_(nullptr) {}

// Getters
std::unordered_map<std::string, std::string> HttpRequest::getHeaders(void) const
{return (headers_);}

std::string HttpRequest::getHttpRequest(void) const
{return (httpRequest_);}

std::string HttpRequest::getMethod(void) const
{return (method_);}

std::string HttpRequest::getUrl(void) const
{return (url_);}

std::string HttpRequest::getVersion(void) const
{return (version_);}

int HttpRequest::getPort(void) const
{return (port_);}

// Setters
void HttpRequest::setHttpRequest(std::string req)
{httpRequest_ = req;}

void HttpRequest::setMethod(std::string meth)
{method_ = meth;}

void HttpRequest::setUrl(std::string url)
{url_ = url;}

void HttpRequest::setVersion(std::string ver)
{version_ = ver;}

static bool isOnlyWhitespace(const std::string& str)
{
	for (char c : str)
	{
		if (!std::isspace(static_cast<unsigned char>(c)))
			return false;
	}
	return true;
}

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
void	HttpRequest::parseMethod(std::string& line)
{
	std::string::size_type	firstSpace;
	std::string				method;

	httpRequest_ = line;
	firstSpace = line.find(' ');
	method = line.substr(0, firstSpace);
	if (isValidMethod(method))
		method_ = method;
	else
		method_ = "UKNOWN";
}

void	HttpRequest::parseUrl(std::string& line)
{
	std::string::size_type	firstSpace;
	std::string				url;

	line = removeFirstWord(line);
	firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
	{
		std::cout << "no url provided\n";
		url_ = "";
	}
	else
		url_ = line.substr(0, firstSpace);
}

void	HttpRequest::parseHttpVersion(std::string& line)
{
	std::string				version;

	version = removeFirstWord(line);
	if (version.size() == 0)
		std::cout << "no version provided\n";
	else
		version_ = version;
}

void	HttpRequest::parseRequestLine(std::string& line)
{
	parseMethod(line);
	parseUrl(line);
	parseHttpVersion(line);
}

void	HttpRequest::parseLine(std::string line)
{
	std::string::size_type	colonPos;
	colonPos = line.find(':');
	if (colonPos != std::string::npos)
	{
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1, std::string::npos);
		headers_.insert(std::pair<std::string, std::string>(key, value));
	}
	// else
	// {
	// 	std::cout << RED << "No colon after key, no idea what to do" << QUIT << std::endl;
	// 	return ;
	// }
}

void	HttpRequest::fillBody(std::string& requestLine)
{
	bodyComplete_ = requestLine;
	std::string	element;

	while (!requestLine.empty())
	{
		size_t posFound = requestLine.find('&');
		if (posFound != std::string::npos)
		{
			element = requestLine.substr(0, posFound);
			requestLine = requestLine.substr(posFound + 1);
		}
		else
		{
			posFound = requestLine.find("\r\n");
			if (posFound != std::string::npos)
			{
					element = requestLine.substr(0, requestLine.find("\r\n"));
					requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
			}
			else
			{
				element = requestLine;
				requestLine.clear();
			}
		}
		bodyVector_.push_back(element);
	}
}

void	HttpRequest::readRequest(const std::string& req)
{
	std::string requestLine = req;
	int body = 0;
	std::string	line = requestLine.substr(0, requestLine.find("\r\n"));

	if (!line.empty() && line.size() > 0)
		parseRequestLine(line);
	requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
	line = requestLine.substr(0, requestLine.find("\r\n"));
	while ((!line.empty() && line.size() > 0) || body == 0)
	{
		parseLine(line);
		requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
		line = requestLine.substr(0, requestLine.find("\r\n"));
		if (line.empty() || line.size() == 0)
		{
			requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
			fillBody(requestLine);
				body = 1;
		}
		else if (requestLine.find("\r\n") == std::string::npos)
			throw HttpRequest::httpParserException();
	}
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
bool	HttpRequest::validatePost(void) // what about Connection → Optional; defaults to keep-alive in HTTP/1.1.
{
	auto it = headers_.begin();
	int	contType = 0;
	int	contLength = 0;

	for (it = headers_.begin(); it != headers_.end(); it++)
	{
		if (it->second.empty() || isOnlyWhitespace(it->second))
		{
			if (it->first == "Content-Type")
				std::cout << RED << "Content-Type isn't specified" << QUIT << std::endl;
			else if (it->first == "Content-Length")
				std::cout << RED << "Content-Length isn't specified" << QUIT << std::endl;
			return (false);
		}
		if (it->first == "Content-Type")
			contType++;
		else if (it->first == "Content-Length")
		{
			int i;
			try
			{
				i = stoi(it->second);
			}
			catch (const std::invalid_argument& e)
			{
				std::cout << RED << "Content-Length specifier is not an int" << QUIT << std::endl;
				return (false);
			}
			if (i < 0 || !isOnlyDigit(it->second))
			{
				std::cout << RED << "Content-Length is not acceptable: " << it->second << QUIT << std::endl;
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
	if (method_ == "UKNOWN" || url_.empty() || version_.empty())
	{
		if (method_ == "UKNOWN")
			std::cout << RED << "Can't handle uknown method->400 Bad Request" << QUIT << std::endl;
		if (url_.empty())
			std::cout << RED << "No url->server should be closed by foreign host" << QUIT << std::endl;
		if (version_.empty())
			std::cout << RED << "No HTTP version->server should be closed by foreign host" << QUIT << std::endl;
		std::cout << RED << "Remove the above after debugging" << QUIT << std::endl;
		return (false);
	}
	auto it = headers_.begin();
	for (it = headers_.begin(); it != headers_.end(); it++)
		if (it->first == "Host")
			break;
	if (it == headers_.end() || (it->second).empty() || isOnlyWhitespace(it->second))
	{
		std::cout << RED << "Didn't find Host" << QUIT << std::endl;
		return (false);
	}
	if (method_ == "POST" && !validatePost())
	{
		std::cout << RED << "Post could not be validated" << QUIT << std::endl;
		return (false);
	}
	// if (method_ == "DELETE") //NOT YET I THINK
	// 	if (!validateDelete())
	return (true);
}

void	HttpRequest::extractPortFromHost()
{
	auto	it = headers_.begin();
	for (it = headers_.begin(); it != headers_.end(); it++)
		if (it->first == "Host")
			break;

	if (it == headers_.end())
		return ;
	std::string	host = it->second;
	auto		pos = host.find(':');
	if (pos == std::string::npos)
	{
		port_ = 80;
		return ;
	}
	std::string	port = it->second.substr(pos + 1, std::string::npos);
	int			portNbr;
	try {
		portNbr = stoi(port);
	}
	catch (const std::invalid_argument& e) {
		std::cout << RED << "Exception thrown: Couldn't convert port to int" << QUIT << std::endl;
		return;
	}

	if (portNbr > 0 && portNbr < 65536) // NOT SURE ABOUT THIS CHECK!!!
		port_ = portNbr;
	else
		std::cout << RED << "Consider choosing another port instead of PORT:" << portNbr << QUIT << std::endl;
}

// Debug
void	HttpRequest::printRequest(void) const
{
	std::cout << MAGENTA << "Printing request: {" << std::endl;
	std::cout << httpRequest_ << std::endl;
	std::cout << "method:\t\t"<< method_ << std::endl;
	std::cout << "url:\t\t"<< url_ << std::endl;
	std::cout << "version:\t"<< version_ << std::endl;
	std::cout << "PORT:\t\t"<< port_ << "}" << QUIT << std::endl;
	std::cout << std::endl;
}

void	HttpRequest::printHeaders(void) const
{
	std::cout << CYAN << "Printing headers: {" << std::endl;
	for (const auto& pair : headers_)
		std::cout << pair.first << ":\t" << pair.second << "}" << std::endl;;
	std::cout << QUIT << std::endl;
}

void	HttpRequest::printBody(void) const
{
	std::cout << BLUE << "Printing whole body: {" << std::endl;
	std::cout << bodyComplete_ << std::endl;
	std::cout << "Printing body elements: " << std::endl;
	for (auto it = bodyVector_.begin(); it != bodyVector_.end(); it++)
		std::cout << "'" << *it << "' ";
	std::cout << "}" << QUIT << std::endl << std::endl;
}

const char *HttpRequest::httpParserException::what() const throw()
{
	return ("Exception thrown: error while parsing http request.");
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