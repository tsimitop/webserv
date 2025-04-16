#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Cgi
{
private:
	int			pipe_fd_[2];
	int			poll_fd_;
	bool		executable_; // can it be executed
	bool		timed_out_;
	HttpRequest	cgi_request_;
	HttpRequest	cgi_response_;
	Cgi() = delete;
	Cgi(const Cgi& other) = delete;
	Cgi& operator=(const Cgi& other) = delete;
public:
	Cgi(int poll_fd, const HttpRequest& request);
	~Cgi();
};
