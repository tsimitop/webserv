#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <csignal>
//===================READ & WRITEL=======================================
#include <unistd.h>
//===================FILE CONTROL========================================
#include <fcntl.h>
//===================SOCKETS & POLL======================================
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
//===================NEW LIBRARIES=======================================
#include <netinet/in.h>
#include <netdb.h>
//===================CONFIG | REPOST | REQUEST===========================
#include "../config/Http.hpp"
#include "../http_requests/HttpRequest.hpp"
#include "../http_requests/HttpResponse.hpp"
#include "../cgis/CgiSingleton.hpp"
//===================ENUMERATORS=========================================

enum serverOrClient{NON_SETTED = -1, SERVER = 0, CLIENT = 1};
enum stateOfPoll{FIRST_TIME, NOT_FIRST_TIME};
struct PollFdWithFlag
{
	pollfd 				pollfd_;
	int					state_;
	int					type_;
	int					method_is_finished_;
	std::string 		final_buffer_;
	std::string 		final_resp_buffer_;

	HttpRequest			req_;
	HttpResponse		resp_;
	size_t				real_max_body_size_ln_;
	size_t				content_length_;
	std::string			file_type_;
	size_t				timeout_;

	std::vector<size_t> connected_fds_;
	ServerInfo			connected_server_; 
//===================DEFAULT CONSTRUCTORS================================
	PollFdWithFlag();
	PollFdWithFlag(const PollFdWithFlag& other);
	// PollFdWithFlag(PollFdWithFlag&& other) noexcept = default;
	// PollFdWithFlag& operator=(PollFdWithFlag&& other) noexcept = default;
	PollFdWithFlag(
		pollfd temp_fd, 
		int state,
		int type,
		int post_is_finished,
		std::string final_buffer,
		std::string final_resp_buffer,
		HttpRequest req,
		HttpResponse	resp,
		size_t real_max_body_size_ln,	
		size_t content_length,
		std::string file_type,
		size_t timeout,
		std::vector<size_t> connected_fds,
		ServerInfo connected_server
	);
	PollFdWithFlag& operator=(const PollFdWithFlag& other);
	~PollFdWithFlag();
//===================SETTERS==============================================
	void setFinalBuffer(const std::string& other_final);
	void setPollFd(const pollfd& other_fd);
	void setRequest(const HttpRequest req);
	void setRealMaxBodySizeLn(const size_t& real_max_body_size_ln);
	void setContentLength(int bytes, char buffer[]);
	void setConnectedFds(const std::vector<size_t>& connected_fds);
	void setConnectedServer(const ServerInfo& connected_server);
	void setFinalRespBuffer();
	void setFinalRespBufferIfCgi();
};