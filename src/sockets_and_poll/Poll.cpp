#include "../../inc/sockets_and_poll/Poll.hpp"

//===============DEFAULTS========================================================
Poll::Poll():
poll_success_flag_(YES), 
max_queued_clients_(5), 
fds_(),
fds_with_flag_(),
server_fd_(0), 
addr_(),
config_(),
number_of_active_servers_(0)
{};

Poll::Poll(const Poll& other)
{
	poll_success_flag_ = other.poll_success_flag_;
	max_queued_clients_ = other.max_queued_clients_;
	fds_ = other.fds_;
	fds_with_flag_ = other.fds_with_flag_;
	server_fd_ = other.server_fd_;
	addr_ = other.addr_;
	config_ = other.config_;
	number_of_active_servers_ = other.number_of_active_servers_;
};

Poll& Poll::operator=(const Poll& other)
{
	if (this != &other)
	{
		poll_success_flag_ = other.poll_success_flag_;
		max_queued_clients_ = other.max_queued_clients_;
		fds_ = other.fds_;
		fds_with_flag_ = other.fds_with_flag_;
		server_fd_ = other.server_fd_;
		addr_ = other.addr_;
		config_ = other.config_;
		number_of_active_servers_ = other.number_of_active_servers_;
	}
	return *this;
};

Poll::~Poll(){};
//===============THE SETTER (THE ONLY ONE)=======================================
void Poll::setConfig(const Http& new_config)
{
	config_ = new_config;
};
//===============BINDING && SYNCHRONUS I/O ======================================
int Poll::binding()
{
	int poll_success_flag_ = NO;
	for (ServerInfo& s : config_.active_servers_)
	{
			addrinfo temp, *res;
			memset(&temp, 0, sizeof(temp));
			std::string port_char_pointer = std::to_string(s.listen_);
			temp.ai_family = AF_UNSPEC;
			temp.ai_socktype = SOCK_STREAM;
			temp.ai_flags = AI_PASSIVE;
		
			if (!s.server_name_.empty() && !port_char_pointer.empty())
			{
				if (getaddrinfo(s.server_name_.c_str(), port_char_pointer.c_str(), &temp, &res) != 0)
				{
					std::cerr << RED << "Error: Get Address Info Failed!" << QUIT<<std::endl;
					freeaddrinfo(res);
					continue;
				};
			}
			else
			{
				std::cerr << RED << "The port or host_name is empty!" << QUIT<<std::endl;
				continue;
			}
			server_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (server_fd_ == -1)
			{
				std::cerr << RED << "Error: Socket Failed!" << QUIT<<std::endl;
				freeaddrinfo(res);
				continue;
			}
			setNonBlockingFd(server_fd_);
			int opt = 1;
			if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			{
				std::cerr << RED << "Error: SockOpt failed!" << QUIT<<std::endl;
				freeaddrinfo(res);
				continue;
			};
			if (bind(server_fd_,res->ai_addr,res->ai_addrlen) == -1)
			{
				std::cerr << RED << "Error: Bind failed!" << QUIT<<std::endl;
				freeaddrinfo(res);
				continue;
			};
			listen(server_fd_, max_queued_clients_);
			fds_with_flag_.push_back(
				(PollFdWithFlag){{server_fd_, POLLIN, 0}, 
				FIRST_TIME,
				SERVER,
				{}, 
				(HttpRequest){"", s}, 
				0, 0, 0,
				{}, s});
			std::cout << CYAN << "Server: fd[" << server_fd_ << "] port: " << s.listen_ << QUIT << std::endl;
			// freeing the addrinfo
			freeaddrinfo(res);
			number_of_active_servers_++;
			poll_success_flag_ +=YES;
		}
	return poll_success_flag_;
};
//===============POLL CALL ======================================================
int Poll::polling()
{
	fds_.clear();
		// is running the pollfds
		for (size_t i = 0; i != fds_with_flag_.size(); i++)
			fds_.push_back(fds_with_flag_[i].pollfd_);
		int activity = poll(
							fds_.data(),
							fds_.size(), 
							-1
						);
		if (activity == 0)
		{
			std::cerr << RED <<  "Error: Poll failed or Timed out!" << QUIT <<  std::endl;
			return activity;
		}
		for (size_t i = 0; i != fds_with_flag_.size(); i++)
			 fds_with_flag_[i].pollfd_ =fds_[i];
		return activity;
};

void	Poll::connecting()
{
	for (size_t i = 0; i != config_.active_servers_.size(); i++)
	{
		if (fds_with_flag_[i].pollfd_.revents & POLLIN)
		{
			int client_fd = accept(fds_with_flag_[i].pollfd_.fd, NULL, NULL);
			if (client_fd == -1)
				continue ;
			else
			{
				setNonBlockingFd(client_fd);
				fds_with_flag_.push_back(
					(PollFdWithFlag){(pollfd){client_fd, POLLIN, 0}, 
					FIRST_TIME,
					CLIENT,
					{}, {}, 
					0, 0, 0,
					{(size_t)fds_with_flag_[i].pollfd_.fd}, config_.active_servers_[i]}); // saving with which port they are connected and in which server
				std::cout <<YELLOW  << "Client: " << client_fd << " connected to [" << fds_with_flag_[i].pollfd_.fd << "]" << QUIT<< std::endl;
			}
			fds_with_flag_[i].connected_fds_.push_back((size_t)client_fd);
		}
	}
};

void Poll::synchroIO()
{
	//--------- testing the invalid host and timeout and max bodylength
	while(YES)
	{
		int activity = polling();
		if (activity == -1)
		{
			if (errno == EINTR)
				std::cerr<< RED << "Error: poll failed! Interupted signal!" << QUIT << std::endl;
			if (errno == EINVAL)
				std::cerr<< RED << "Error: poll failed!File dirrectories exceeding the max fds or wrong time out value!" << QUIT << std::endl;
			if (errno == ENOMEM)
				std::cerr << RED << "Error: poll failed! Not allocation enough space!" << QUIT << std::endl;
			break;
		}
		else if(activity == 0)
			break;
		connecting();
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			int er = errno;
			(void)er;
			pollhup(i);
			if (pollin(i) == NO)
				continue;
			pollout(i);
		}
	}
	closingServers();
};
//===============POLL STATES ====================================================
void		Poll::pollhup(size_t& i)
{
	std::string poll_err = fds_with_flag_[i].pollfd_.revents & POLLERR ? "POLLERR:" :
							fds_with_flag_[i].pollfd_.revents & POLLERR ?  "POLLHUP: " : 
							"POLLNVAL: ";
	if (fds_with_flag_[i].pollfd_.revents & (POLLERR | POLLHUP | POLLNVAL))
		disconecting(i, poll_err);
};


int	Poll::pollin(size_t i)
{
	int answer = YES;
	if (fds_with_flag_[i].pollfd_.revents & (POLLIN))
	{
		size_t temp_len = lengthProt(i);
		char* buffer = new char[lengthProt(i) + 1];
		
		int bytes = recv(fds_with_flag_[i].pollfd_.fd, buffer, temp_len, 0);
		if (bytes == 0 || bytes < 0)
			answer = eAgainAndEWouldblock(i, bytes);
		else
		{
			if ((size_t)bytes > temp_len)
			{
				std::cerr << "not valid config or sockets!\n";
				fds_with_flag_[i].pollfd_.events = POLLERR;
			}
			if (fds_with_flag_[i].content_length_ == 0 && bytes > 0)
				fds_with_flag_[i].setContentLength(bytes,buffer);
			//------------
			size_t final_buffer_len = fds_with_flag_[i].final_buffer_.length();
			size_t content_len = fds_with_flag_[i].content_length_;
			//-------------
			if(temp_len ==(size_t)bytes || (content_len > (size_t)bytes + final_buffer_len && content_len !=0))
				{
					answer = updateFinalBuffer(i, bytes, buffer);
				}
			else
			{
				buffer[bytes] = '\0';
				updateFinalBuffer(i, bytes, buffer);
				definingRequest(i);
			}
		}
		if (buffer)
			delete[] buffer;
	}
	return answer;
};

void		Poll::pollout(size_t i)
{
	HttpResponse resp;
	if(fds_with_flag_[i].pollfd_.events & POLLOUT)
	{
		HttpResponse response;
		response = fds_with_flag_[i].req_.performMethod();
		std::string resp = response.respond(fds_with_flag_[i].req_);
		std::cout << GREEN << resp << std::endl << QUIT;
		int act = send(fds_with_flag_[i].pollfd_.fd, resp.c_str(), resp.length(), 0);
		if (act < 0)
		{
			fds_with_flag_[i].pollfd_.events = POLLERR; // operation failed
		}
		else
		{
			close(fds_with_flag_[i].pollfd_.fd);
			fds_with_flag_[i].pollfd_.events = POLLHUP; //closing the socket
		}
	}
};
//================HELPER METHODS ================================================
void		Poll::closingServers()
{
	if (!fds_with_flag_.empty())
	{
		while (!fds_with_flag_.empty())
		{
			close(fds_with_flag_[0].pollfd_.fd);
			fds_with_flag_.erase(fds_with_flag_.begin());
		}
	}
};

size_t		Poll::lengthProt(size_t i)
{
	size_t max_body_size = (size_t)fds_with_flag_[i].connected_server_.client_max_body_size_;
	// size_t max_body_size = 0;
	// size_t max_body_size = -1;
	
	return (max_body_size);
};

void		Poll::setMaxBodyLen(size_t i, int bytes)
{
		if (fds_with_flag_[i].state_ == FIRST_TIME && bytes > 0)
		{
			int port = fds_with_flag_[i].req_.getPort();
			for(ServerInfo& s : config_.active_servers_)
				if (s.listen_ == port)
					fds_with_flag_[i].setRealMaxBodySizeLn((size_t)s.client_max_body_size_);
			fds_with_flag_[i].state_ = NOT_FIRST_TIME;
		}
};

void		Poll::definingRequest(size_t i)
{
			fds_with_flag_[i].req_.readRequest(fds_with_flag_[i].final_buffer_);
			fds_with_flag_[i].req_.setCurrentServer(fds_with_flag_[i].connected_server_);
			
			fds_with_flag_[i].pollfd_.events |= POLLOUT;
};

void		Poll::disconecting(size_t& i, std::string str)
{
	size_t fd_of_server = fds_with_flag_[i].connected_fds_[0];
	close(fds_with_flag_[i].pollfd_.fd);
	std::cout << BLUE << "Client : " << fds_with_flag_[i].pollfd_.fd << 
	" " << str << "Disconnection from Server " << 
	fd_of_server << QUIT << std::endl;
	fds_with_flag_.erase(fds_with_flag_.begin() + i);
	i--;
}

//===================OUTER HELPER FUNCTIONS ==========================================
int setNonBlockingFd(int fd)
{
	//not an extra parameter 0
	int flag = fcntl(fd, F_GETFL);
	return (fcntl(fd, F_SETFL, flag | O_NONBLOCK));
}

std::string filteredBuffer(std::string buffer, int& steps_back)
{
	std::string new_buffer;
	int i = -1;
	while (buffer[++i])
	{
		if (isprint(buffer[i]) && !isspace(buffer[i]))
			new_buffer += buffer[i];
		else
			steps_back++;
	}
	return new_buffer;
}

std::string safeNullTermination(int bytes, size_t max_body_ln, std::string buffer)
{
	std::string res;
	for (size_t i = 0; i != max_body_ln; i++)
		res[i] = buffer[i];
	if ((size_t)bytes < max_body_ln)
		res[(size_t)bytes] = '\0';
	else
		res[max_body_ln - 1] = '\0';
	return res;
};


int			Poll::eAgainAndEWouldblock(size_t i, int bytes)
{
	if (bytes == 0)
	{
		std::cout << "This string is empty!\n";
		close(fds_with_flag_[i].pollfd_.fd);
		fds_with_flag_[i].pollfd_.events = POLLOUT;
		// disconecting(i, "(EOF)");
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return NO;
		else if (errno == EINVAL)
		{
			close(fds_with_flag_[i].pollfd_.fd);
			fds_with_flag_[i].pollfd_.events = POLLOUT;
		}
	}
			// disconecting(i, "(recv)"); // EINVAL ECONNECTRESET ENOTCONN
	return NO;
};
int			Poll::updateFinalBuffer(size_t i, int bytes, char buffer[])
{
	size_t l = 0;
	while (l != (size_t)bytes)
	{
		fds_with_flag_[i].final_buffer_.push_back(buffer[l]);
		l++;
	}
	return NO;
};
