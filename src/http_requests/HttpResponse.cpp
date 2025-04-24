#include "../../inc/http_requests/HttpResponse.hpp"

HttpResponse::HttpResponse() : statusCode_(0), reasonPhrase_("Empty")
{
	statusReason_ = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{103, "Early Hints"}, // for preloading resources,
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Payload Too Large"},
		{414, "URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{422, "Unprocessable Content"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{451, "Unavailable For Legal Reasons"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{507, "Insufficient Storage"},
		{511, "Network Authentication Required"}
	};
}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		this->statusReason_ = other.statusReason_;
		this->statusCode_ = other.statusCode_;
		this->reasonPhrase_ = other.reasonPhrase_;
		this->contentType_ = other.contentType_;
		this->contentLength_ = other.contentLength_;
		this->body_ = other.body_;
	}
	return (*this);
}

HttpResponse::~HttpResponse() {}

// Parameterized constructor
HttpResponse::HttpResponse(int sc)
{
	statusReason_ = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{103, "Early Hints"},
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},
		{404, "error on Wikimedia"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Payload Too Large"},
		{414, "URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot // not applicable but fun to keep"},
		{422, "Unprocessable Content"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{451, "Unavailable For Legal Reasons"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{507, "Insufficient Storage"},
		{511, "Network Authentication Required"}
	};
	statusCode_ = sc;
	auto it = statusReason_.begin();
	for (it = statusReason_.begin(); it != statusReason_.end(); it++)
		if (it->first == statusCode_)
			break;
	if (it != statusReason_.end())
		reasonPhrase_ = it->second;
	else
		reasonPhrase_ = "Uknown Reason Phrase";
}

// Getters
int			HttpResponse::getStatusCode(void) const {return (statusCode_);}
std::string	HttpResponse::getReasonPhrase(void) const {return (reasonPhrase_);}
std::string	HttpResponse::getContentType(void) const {return (contentType_);}
std::string	HttpResponse::getBody(void) const {return (body_);}
int			HttpResponse::getContentLength(void) const {return (contentLength_);}

// Setters
void	HttpResponse::setStatusCode(int sc) {statusCode_ = sc;}
void	HttpResponse::setContentLength(int len) {contentLength_ = len;}
void	HttpResponse::setContentType(std::string ctype) {contentType_ = ctype;}
void	HttpResponse::setBody(const std::string& body) {body_ = body;}

void	HttpResponse::setReasonPhrase(int sc)
{
	auto it = statusReason_.begin();
	for (it = statusReason_.begin(); it != statusReason_.end(); it++)
	{
		if (sc == it->first)
			break;
	}
	reasonPhrase_ = it->second;
}

const std::string HttpResponse::respond(const HttpRequest& req)
{
	time_t		timestamp;
	std::stringstream temp;
	std::string response;

	time(&timestamp);
	response += req.getVersion();
	response += " ";
	response += std::to_string(this->getStatusCode());
	response += " ";
	response += this->getReasonPhrase();
	response += "\r\n";
	response += "Server: Webserv\n";
	response += "Date: ";
	response += ctime(&timestamp);
	response += "Content-Length: ";
	temp << this->getContentLength();
	std::string lengthString;
	temp >> lengthString;
	response += lengthString;
	response += "\r\n";
	if (req.isRedirection())
	{
		response += getBody();
		response += "\r\n";
		if (this->getStatusCode() == 301)
			response += "Connection: close\r\n";
		else
			response += "Connection: keep-alive\r\n";
		response += "\r\n";
		return (response);
	}
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: ";
	response += this->getContentType();
	response += "\r\n";
	response += "\r\n";
	response += this->getBody();
	response += "\r\n";
	response += "\r\n";
	return (response);
}

void HttpResponse::createResponse(int status_code, std::filesystem::path file)
{
	std::ifstream input_file(file.string());
	if (!input_file.is_open())
		std::cout << RED << "Could not open error file: " << file.string() << QUIT << std::endl;
	setStatusCode(status_code);
	setReasonPhrase(status_code);
	setContentType("text/html");
	std::stringstream ss;
	ss << input_file.rdbuf();
	input_file.close();
	std::string temp;
	temp = ss.str();
	setContentLength(temp.length());
	setBody(temp);
}

void HttpResponse::createCgiResponse(int status_code, std::string content)
{
	setStatusCode(status_code);
	setReasonPhrase(status_code);
	setContentType("text/html");
	std::string body = "<!DOCTYPE html>\n<html>\n<body>\n<p>";
	body += content;
	body += "</p>\n</body>\n</html>";
	setContentLength(body.size());
	setBody(body);
}

void HttpResponse::redirResponse(Location& location)
{
	setStatusCode(location.redir_status_);
	setReasonPhrase(location.redir_status_);
	setContentLength(0);
	std::string location_to_redir = "Location: " + location.redir_location_;
	setBody(location_to_redir);
}
