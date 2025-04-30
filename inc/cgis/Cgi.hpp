#pragma once

#include "../http_requests/HttpRequest.hpp"
#include "../http_requests/HttpResponse.hpp"
#include "CgiSingleton.hpp"
#include <chrono>
#include <cstring>

class Cgi
{
private:
	std::map<int, std::filesystem::path>			available_errors_;
	int												status_;
	int												pipe_fd_[2];
	int												poll_fd_;
	bool											cgi_is_executable_; // can it be executed
	bool											timed_out_;
	bool											has_forked_;
	HttpRequest										cgi_request_;
	HttpResponse									cgi_response_;
	bool											exec_complete_;
	std::filesystem::path							www_path_;
	std::string										url_;
	std::filesystem::path							path_of_program_to_execute_;
	std::string										executable_;
	std::string										language_;
	pid_t											pid_;
	std::string										response_body_;
	std::chrono::milliseconds									timeout_total_;
	std::chrono::time_point<std::chrono::high_resolution_clock>	procces_start_;
	// const HttpRequest*								request_ptr_;
	Cgi() = delete;
public:
	Cgi& operator=(const Cgi& other);
	Cgi(const Cgi& other);
	Cgi(int poll_fd, const HttpRequest& request);
	~Cgi();

	void setResponseBody(std::string response_str);

	void execute();
	void execution_close();
	int getPollFd() const;
	pid_t getPid() const;
	int getStatus() const;
	int getFdOne() const;
	std::string getRespBody() const;
	bool isExecutable() const;
	bool hasTimedOut() const;
	bool hasForked() const;
	HttpRequest getCgiRequest() const;
	bool cgiPidDone() const;
	
	void setStatus(int status);

	bool performed_wait();
	bool read_pipe();
	void check_timeout();
	HttpResponse response_of_cgi(HttpResponse& resp, HttpRequest req);
};
