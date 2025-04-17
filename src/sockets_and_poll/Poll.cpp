#include "../../inc/sockets_and_poll/Poll.hpp"

//===============DEFAULTS========================================================
Poll::Poll():
socket_success_flag_(YES), 
max_queued_clients_(5), 
fds_(),
fds_with_flag_(),
server_fd_(0), 
addr_(),
config_()
{};

Poll::Poll(const Poll& other)
{
	socket_success_flag_ = other.socket_success_flag_;
	max_queued_clients_ = other.max_queued_clients_;
	fds_ = other.fds_;
	fds_with_flag_ = other.fds_with_flag_;
	server_fd_ = other.server_fd_;
	addr_ = other.addr_;
	config_ = other.config_;
};

Poll& Poll::operator=(const Poll& other)
{
	if (this != &other)
	{
		socket_success_flag_ = other.socket_success_flag_;
		max_queued_clients_ = other.max_queued_clients_;
		fds_ = other.fds_;
		fds_with_flag_ = other.fds_with_flag_;
		server_fd_ = other.server_fd_;
		addr_ = other.addr_;
		config_ = other.config_;
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
	int socket_success_flag_ = YES;
	for (ServerInfo s : config_.servers_)
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
				std::cerr << "Error: Get Address Info Failed!\n";
				freeaddrinfo(res);
				socket_success_flag_ = NO;
				continue;
			};
		}
		else
		{
			std::cerr << "The port or host_name is empty!\n";
			socket_success_flag_ = NO;
			continue;
		}
		server_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (server_fd_ == -1)
		{
			std::cerr << "Error: Socket Failed!\n";
			freeaddrinfo(res);
			socket_success_flag_ = NO;
			continue;
		}
		setNonBlockingFd(server_fd_);
		int opt = 1;
		if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			std::cerr << "Error: SockOpt failed!\n";
			freeaddrinfo(res);
			socket_success_flag_ = NO;
			continue;
		};
		if (bind(server_fd_,res->ai_addr,res->ai_addrlen) == -1)
		{
			std::cerr << "Error: Bind failed!\n";
			freeaddrinfo(res);
			socket_success_flag_ = NO;
			continue;
		};
		listen(server_fd_, max_queued_clients_);
		// fds_.push_back((pollfd){server_fd_, POLLIN, 0});
		fds_with_flag_.push_back((PollFdWithFlag){{server_fd_, POLLIN, 0}, FIRST_TIME, {}, {}, 0});
		std::cout << "Server is listening to the port: " << s.listen_ << "\n";
		// freeing the addrinfo
		freeaddrinfo(res);
	}
	return socket_success_flag_;
};
//===============POLL CALL ======================================================
int Poll::polling()
{
	fds_.clear();
		// is running the pollfds
		for (size_t i = 0; i != fds_with_flag_.size(); i++)
			fds_.push_back(fds_with_flag_[i].fd_);
		int activity = poll(fds_.data(),fds_.size(), config_.servers_[0].server_timeout_);
		if (activity == -1)
		{
			std::cerr << "Error: Poll failed or Timed out!\n";
			return activity;
		}
		for (size_t i = 0; i != fds_with_flag_.size(); i++)
			 fds_with_flag_[i].fd_ =fds_[i];
		return activity;
};

void	Poll::connecting()
{
	for (size_t i = 0; i != config_.servers_.size(); i++)
	{
		// struct here
		if (fds_with_flag_[i].fd_.revents & POLLIN)
		{
			//struct here
			int client_fd = accept(fds_with_flag_[i].fd_.fd, NULL, NULL);
			if (client_fd == -1)
				continue ;
			else
			{
				// PollFdWithFlag temp_poll_fd_with_flag;
				setNonBlockingFd(client_fd);
				fds_with_flag_.push_back((PollFdWithFlag){(pollfd){client_fd, POLLIN, 0}, FIRST_TIME, {}, {}, 0});
				std::cout << "Client Connected : " << client_fd << "\n";
			}
		}
	}
};

void Poll::synchroIO()
{
	while(YES)
	{
		if (polling() == -1)
			break;
		connecting();
		for (size_t i = config_.servers_.size(); i != fds_with_flag_.size(); i++)
		{
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
	if (fds_with_flag_[i].fd_.revents & (POLLERR | POLLHUP))
		disconecting(i, "(POLLERR | POLLHUP)");
};

int	Poll::pollin(size_t i)
{
	int answer = YES;
	if (fds_with_flag_[i].fd_.revents & (POLLIN))
	{
		char buffer[lengthProt(i) + 1];
		//struct here
		int bytes = recv(fds_with_flag_[i].fd_.fd, buffer, lengthProt(i), 0);
		size_t l = 0;
		setMaxBodyLen(i, buffer, bytes);
		if ((size_t)bytes == lengthProt(i) || bytes == 0 || bytes < 0)
		{
			if ((size_t)bytes == lengthProt(i))
			{
				while (l != lengthProt(i))
				{
					fds_with_flag_[i].final_buffer_.push_back(buffer[l]);
					l++;
				}
			}
			else if (bytes == 0)
				disconecting(i, "(EOF)");
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					;
				else
					disconecting(i, "(recv)");
			}
			answer = NO;
		}
		else
		{
			if (bytes > 0 && (size_t)bytes < lengthProt(i))
				buffer[bytes] = '\0';
			findingPort(l,i,bytes, buffer);
		}
	}
	return answer;
};

void		Poll::pollout(size_t i)
{
	HttpResponse resp;
	if(fds_with_flag_[i].fd_.events & POLLOUT)
	{
		HttpResponse response;
		response = fds_with_flag_[i].req_.performMethod();
		std::string resp = response.respond(fds_with_flag_[i].req_);
		std::cout << GREEN << resp << std::endl << QUIT;
		send(fds_with_flag_[i].fd_.fd, resp.c_str(), resp.length(), 0);
		fds_with_flag_[i].fd_.events = POLLHUP;
	}
};
//================HELPER METHODS ================================================
void		Poll::closingServers()
{
	if (!fds_with_flag_.empty())
	{
		while (!fds_with_flag_.empty())
		{
			close(fds_with_flag_[0].fd_.fd);
			fds_.erase(fds_.begin());
		}
	}
};

size_t		Poll::lengthProt(size_t i)
{
	return std::min((size_t)10000000,std::max((size_t)1024, fds_with_flag_[i].real_max_body_size_ln_));
};

void		Poll::setMaxBodyLen(size_t i,char buffer[], int bytes)
{
		if (fds_with_flag_[i].state_ == FIRST_TIME && bytes > 0)
		{
			std::string temp;
			HttpRequest temp_req;
			for (size_t i = 0; i != (size_t)bytes; i++)
				temp.push_back(buffer[i]);
			size_t here = temp.find("Host: localhost:") + 16;
			int port = std::stoul(temp.substr(here, here + 4));
			for(ServerInfo& s : config_.servers_)
				if (s.listen_ == port)
					fds_with_flag_[i].setRealMaxBodySizeLn((size_t)s.client_max_body_size_);
			fds_with_flag_[i].state_ = NOT_FIRST_TIME;
		}
};

void		Poll::findingPort(size_t l, size_t i, int bytes, char buffer[])
{
	while (l != std::min((size_t)bytes, std::max((size_t)1024, fds_with_flag_[i].real_max_body_size_ln_)))
	{
		fds_with_flag_[i].final_buffer_.push_back(buffer[l]);
		l++;
	}
	if (l == std::max((size_t)1024, fds_with_flag_[i].real_max_body_size_ln_))
		fds_with_flag_[i].final_buffer_.push_back('\0');
	std::string request;
	for (size_t j = 0; j!=fds_with_flag_[i].final_buffer_.size(); j++)
		request += fds_with_flag_[i].final_buffer_[j];
	fds_with_flag_[i].req_.readRequest(request);
	for (ServerInfo& s : config_.servers_)
		if (fds_with_flag_[i].req_.getPort() == (s).listen_)
			fds_with_flag_[i].req_.setCurrentServer(s);
	fds_with_flag_[i].fd_.events |= POLLOUT;
};

void		Poll::disconecting(size_t& i, std::string str)
{
	close(fds_with_flag_[i].fd_.fd);
	std::cout << "Client : " << fds_with_flag_[i].fd_.fd << " " << str << "Disconnection!\n";
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