#include "../../inc/http_requests/HttpResponse.hpp"

HttpResponse::HttpResponse() : _statusCode(0), _reasonPhrase("Empty")
{
	_statusReason = {
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
: _statusReason(other._statusReason), _statusCode(other._statusCode), _reasonPhrase(other._reasonPhrase) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		this->_statusReason = other._statusReason;
		this->_statusReason = other._statusReason;
		this->_statusReason = other._statusReason;
	}
	return (*this);
}

HttpResponse::~HttpResponse() {}

// Parameterized constructor
HttpResponse::HttpResponse(int sc)
{
	_statusReason = {
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
	_statusCode = sc;
	auto it = _statusReason.begin();
	for (it = _statusReason.begin(); it != _statusReason.end(); it++)
		if (it->first == _statusCode)
			break;
	if (it != _statusReason.end())
		_reasonPhrase = it->second;
	else
		_reasonPhrase = "Uknown Reason Phrase";
}

// Getters
int			HttpResponse::getStatusCode(void) const {return (_statusCode);}
std::string	HttpResponse::getReasonPhrase(void) const {return (_reasonPhrase);}

// Setters
void	HttpResponse::setStatusCode(int sc) {_statusCode = sc;}
void	HttpResponse::setReasonPhrase(std::string rp) {_reasonPhrase = rp;}
