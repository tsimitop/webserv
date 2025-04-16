#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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

	Cgi() = delete;
	Cgi(const Cgi& other) = delete;
	Cgi& operator=(const Cgi& other) = delete;
public:
	Cgi(int poll_fd, const HttpRequest& request);
	~Cgi();

	void execute();
	int getPollFd() const;
};
