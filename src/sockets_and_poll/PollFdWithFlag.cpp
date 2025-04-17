#include "../../inc/sockets_and_poll/PollFdWithFlag.hpp"

PollFdWithFlag::PollFdWithFlag() : fd_(), state_(FIRST_TIME), final_buffer_(), req_(), real_max_body_size_ln_(0)
{
};
PollFdWithFlag::PollFdWithFlag(const PollFdWithFlag& other)
{
	fd_ = other.fd_;
	state_ = other.state_;
	final_buffer_ = other.final_buffer_;
	req_ = other.req_;
	real_max_body_size_ln_ = other.real_max_body_size_ln_;
};

PollFdWithFlag::PollFdWithFlag(pollfd temp_fd, int s_or_c, std::vector<char> final_buffer, HttpRequest req, size_t real_max_body_size_ln):
fd_(temp_fd), 
state_(s_or_c), 
final_buffer_(final_buffer),
req_(req),
real_max_body_size_ln_(real_max_body_size_ln)
{};
PollFdWithFlag& PollFdWithFlag::operator=(const PollFdWithFlag& other)
{
	if (this != &other)
	{
		fd_ = other.fd_;
		state_ = other.state_;
		final_buffer_ = other.final_buffer_;
		req_ = other.req_;
		real_max_body_size_ln_ = other.real_max_body_size_ln_;
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
void PollFdWithFlag::setRealMaxBodySizeLn(const size_t& real_max_body_size_ln) // maximum between 1024 and 10000000
{
	real_max_body_size_ln_ = std::min((size_t)10000000,(size_t)std::max((size_t)1024, real_max_body_size_ln));
};