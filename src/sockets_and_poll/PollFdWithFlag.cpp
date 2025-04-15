#include "../../inc/sockets_and_poll/PollFdWithFlag.hpp"

PollFdWithFlag::PollFdWithFlag() : fd_(), s_or_c_(SERVER)
{
};
PollFdWithFlag::PollFdWithFlag(const PollFdWithFlag& other)
{
	fd_ = other.fd_;
	s_or_c_ = other.s_or_c_;
};

PollFdWithFlag::PollFdWithFlag(pollfd temp_fd, int s_or_c):fd_(temp_fd), s_or_c_(s_or_c){};
PollFdWithFlag& PollFdWithFlag::operator=(const PollFdWithFlag& other)
{
	if (this != &other)
	{
		fd_ = other.fd_;
		s_or_c_ = other.s_or_c_;
	}
	return *this;
};
PollFdWithFlag::~PollFdWithFlag()
{
};