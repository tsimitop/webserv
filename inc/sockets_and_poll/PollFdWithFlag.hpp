#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
//--------READ && WRITE-------------------------
#include <unistd.h>
//--------FILE CONTROL--------------------------
#include <fcntl.h>
//--------POLL && SOCKETS-----------------------
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
//--------NET LIBRARIES------------------------
#include <netinet/in.h>
#include <netdb.h>
//---------CONFIG | RESPONCE | REQUSET---------
#include "../config/Http.hpp"
#include "../http_requests/HttpRequest.hpp"
#include "../http_requests/HttpResponse.hpp"

enum serverOrClient{SERVER = 0, CLIENT = 1};
enum stateOfPoll{READING, WRITING, DONE};
struct PollFdWithFlag
{
	pollfd 				fd_;
	int					state_;
	std::vector<char> 	final_buffer_;
	HttpRequest			req_;

	PollFdWithFlag();
	PollFdWithFlag(const PollFdWithFlag& other);
	PollFdWithFlag(pollfd temp_fd, int s_or_c, std::vector<char>final_buffer, HttpRequest req);
	PollFdWithFlag& operator=(const PollFdWithFlag& other);
	~PollFdWithFlag();
	//helping methods
	void setFinalBuffer(const std::vector<char> & other_final);
	void setPollFd(const pollfd& other_fd);
	void setRequest(const HttpRequest req);
	//from char vector to string
};