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
}

HttpResponse::HttpResponse(const HttpResponse& other)
: statusReason_(other.statusReason_), statusCode_(other.statusCode_), reasonPhrase_(other.reasonPhrase_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		this->statusReason_ = other.statusReason_;
		this->statusReason_ = other.statusReason_;
		this->statusReason_ = other.statusReason_;
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

// Setters
void	HttpResponse::setStatusCode(int sc) {statusCode_ = sc;}

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
	std::string	request = req.getHttpRequest(); // check out the request and figure out response
	std::string	version = req.getVersion(); // check out the request and figure out response
	time_t		timestamp;
	
	// FIGURE OUT RESPONSE!

	// assuming response is 200
	setStatusCode(200);
	setReasonPhrase(200);
	time(&timestamp);

	std::cout << RED << "Responding...\n";
	std::string response;
	response += version;
	response += " ";
	response += std::to_string(this->getStatusCode());
	response += " ";
	response += this->getReasonPhrase();
	response += "\n";
	response += "Server: Webserv\n";
	response += "Date: "; // figure it out
	response += ctime(&timestamp); // figure it out
	response += "\n";
	response += "Content-Type: text/html\n"; // figure it out
	response += "Content-Length: !number!\n"; // figure it out
	response += "Last-Modified: !date!\n"; // figure it out

	std::cout << response << std::endl;
	std::cout << RED << "Send entire file or error\n" << QUIT;
	return (response);
}
