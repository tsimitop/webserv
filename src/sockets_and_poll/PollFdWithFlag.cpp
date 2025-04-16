#include "../../inc/sockets_and_poll/PollFdWithFlag.hpp"

PollFdWithFlag::PollFdWithFlag() : fd_(), state_(READING), final_buffer_(), req_()
{
};
PollFdWithFlag::PollFdWithFlag(const PollFdWithFlag& other)
{
	fd_ = other.fd_;
	state_ = other.state_;
	final_buffer_ = other.final_buffer_;
	req_ = other.req_;
};

PollFdWithFlag::PollFdWithFlag(pollfd temp_fd, int s_or_c, std::vector<char> final_buffer, HttpRequest req):
fd_(temp_fd), 
state_(s_or_c), 
final_buffer_(final_buffer),
req_(req)
{};
PollFdWithFlag& PollFdWithFlag::operator=(const PollFdWithFlag& other)
{
	if (this != &other)
	{
		fd_ = other.fd_;
		state_ = other.state_;
		final_buffer_ = other.final_buffer_;
		req_ = other.req_;
	}
	return *this;
};
PollFdWithFlag::~PollFdWithFlag()
{
};
//setters
void PollFdWithFlag::setFinalBuffer(const std::vector<char>& other_final)
{
	final_buffer_ = other_final;
};
void PollFdWithFlag::setPollFd(const pollfd& other_fd)
{
	fd_ = other_fd;
};

void PollFdWithFlag::setRequest(const HttpRequest req)
{
	req_ = req;
};