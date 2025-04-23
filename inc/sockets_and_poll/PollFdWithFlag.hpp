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
//===================ENUMERATORS=========================================

// volatile sig_atomic_t SIGNALS_E = NO;
enum serverOrClient{NON_SETTED = -1, SERVER = 0, CLIENT = 1};
enum stateOfPoll{FIRST_TIME, NOT_FIRST_TIME};
struct PollFdWithFlag
{
	pollfd 				pollfd_;
	int					state_;
	int					type_;
	std::string 		final_buffer_;
	HttpRequest			req_;

	size_t				real_max_body_size_ln_;
	size_t				content_length_;
	size_t				timeout_;

	std::vector<size_t> connected_fds_;
	ServerInfo			connected_server_; 
//===================DEFAULT CONSTRUCTORS================================
	PollFdWithFlag();
	PollFdWithFlag(const PollFdWithFlag& other);
	PollFdWithFlag(
		pollfd temp_fd, 
		int state,
		int type,
		std::string final_buffer, 
		HttpRequest req, 
		size_t real_max_body_size_ln,	
		size_t content_length,
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

};