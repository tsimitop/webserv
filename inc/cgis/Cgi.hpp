#pragma once

#include "../http_requests/HttpRequest.hpp"
#include "../http_requests/HttpResponse.hpp"
#include "CgiSingleton.hpp"

// class HttpRequest;
// class HttpResponse;

class Cgi
{
private:
	int						status_;
	int						pipe_fd_[2];
	int						poll_fd_;
	bool					cgi_is_executable_; // can it be executed
	bool					timed_out_;
	HttpRequest				cgi_request_;
	HttpResponse			cgi_response_;
	std::filesystem::path	www_path_;
	std::string				url_;
	std::filesystem::path	path_of_program_to_execute_;
	std::string				executable_;
	pid_t					pid_;
	std::string				response_body_;

	Cgi() = delete;
public:
	Cgi& operator=(const Cgi& other);
	Cgi(const Cgi& other);
	Cgi(int poll_fd, const HttpRequest& request);
	~Cgi();

	void execute();
	int getPollFd() const;
	pid_t getPid() const;
	int getStatus() const;
	int getFdOne() const;
	std::string getRespBody() const;
	
	void setStatus(int status);

	HttpResponse update_status(HttpResponse& resp);
	void parform_wait();
	bool read_pipe();

};
