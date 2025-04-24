#include "../../inc/sockets_and_poll/Poll.hpp"

#define MAX_BUFFER 4096
#define DOCKER 0

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
			// Run with docker
			// if (DOCKER)
			// 	s.server_name_ = "0.0.0.0";
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
							0
						);
		// if (activity == 0)
		// {
		// 	std::cerr << RED <<  "Error: Poll failed or Timed out!" << QUIT <<  std::endl;
		// 	return activity;
		// }
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
					{}, (HttpRequest){"", config_.active_servers_[i]}, 
					0, 0, 0,
					{(size_t)fds_with_flag_[i].pollfd_.fd}, config_.active_servers_[i]}); // saving with which port they are connected and in which server
				std::cout <<YELLOW  << "Client: " << client_fd << " connected to [" << fds_with_flag_[i].pollfd_.fd << "]" << QUIT<< std::endl;
			}
			fds_with_flag_[i].connected_fds_.push_back((size_t)client_fd);
		}
	}
};


// void signalHandler(int)
// {
// 	std::cout << "\ncntl + C was catched and exited respectfully!\n";
// 	SIGNALS_E = YES;
// }
void Poll::synchroIO()
{
	// signal(SIGINT, signalHandler);
	// if (SIGNALS_E) 
	// return ;
	while(YES)
	{
		// signal(SIGINT, signalHandler);
		// if (SIGNALS_E)
		// 	break;
		for (size_t i = config_.servers_.size(); i != fds_with_flag_.size(); i++)
			if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd) && CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd)->cgiPidDone())
					fds_with_flag_[i].pollfd_.events |= POLLOUT;
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
		// else if(activity == 0)
		// 	break;
		connecting();
		// signal(SIGINT, signalHandler);
		// if (SIGNALS_E)
		// 	break;
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			int er = errno;
			(void)er;
			pollhup(i);
			int pollin_int = pollin(i);
			if (pollin_int == NO)
				continue;
			else if (pollin_int == SIG)
				fds_with_flag_[i].pollfd_.fd = POLLHUP;
			// else
			// {
			// 	fds_with_flag_[i].final_buffer_.push_back((char)0x04);
			// 	fds_with_flag_[i].pollfd_.fd |= POLLOUT;
			// }
			pollout(i);
		}
	}
	closingServers();
};
//===============POLL STATES ====================================================
void		Poll::pollhup(size_t& i)
{
	std::string poll_err = fds_with_flag_[i].pollfd_.revents & POLLERR ? "POLLERR:" :
							fds_with_flag_[i].pollfd_.revents & POLLHUP ?  "POLLHUP: " :
							fds_with_flag_[i].pollfd_.revents & POLLNVAL ?  "POLLNVAL: " :
							fds_with_flag_[i].pollfd_.revents &  POLLWRBAND ? " POLLWRBAND: ":
							fds_with_flag_[i].pollfd_.revents &  POLLNLINK ? "POLLNLINK" :
							fds_with_flag_[i].pollfd_.revents & POLLRDNORM ? " POLLRDNORM" :
							"UNCLASSIFIED POLL HUNG UP: ";
	if (fds_with_flag_[i].pollfd_.revents & (POLLERR | POLLHUP | POLLNVAL | POLLWRBAND | POLLNLINK | POLLRDNORM)) // revent = POLLERR & POLLER != 0 || 1 0 0 0 0 , 0 1 0 0 0 0 0
		disconecting(i, poll_err);
};


int	Poll::pollin(size_t i)
{
	int answer = YES;
	if (fds_with_flag_[i].pollfd_.revents & (POLLIN))
	{
		size_t temp_len = lengthProt(i); // max_body_client
		char* buffer = new char[lengthProt(i) + 1]; // dynamic allocation for the buffer
		
		memset(buffer, 0, lengthProt(i) + 1); //
		int bytes = recv(fds_with_flag_[i].pollfd_.fd, buffer, temp_len, 0);
		std::cout << MAGENTA << buffer << QUIT << std::endl;
		if (bytes == 0 || bytes < 0)
			answer = eAgainAndEWouldblock(i, bytes);
		else
		{
			int checking_signals = checkingForSignals(buffer, bytes, fds_with_flag_[i].final_buffer_);
			if (checking_signals == SIG)
				return SIG;
			// else
			// 	return EOF_FLAG;
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
	HttpResponse response;
	if(fds_with_flag_[i].pollfd_.events & POLLOUT)
	{
		std::string response_str;
		if (!fds_with_flag_[i].req_.isCgi())
		{
			response = fds_with_flag_[i].req_.performMethod();
			response_str = response.respond(fds_with_flag_[i].req_);
			std::cout << GREEN << "RESPONSE\n" << response_str << std::endl << QUIT;
			int act = send(fds_with_flag_[i].pollfd_.fd, response_str.c_str(), response_str.length(), 0);
			if (act < 0)
				fds_with_flag_[i].pollfd_.events = POLLERR; // operation failed
			else
			{
				// close(fds_with_flag_[i].pollfd_.fd);
				fds_with_flag_[i].pollfd_.events = POLLHUP ; //closing the socket
			}
		}
		else
		{
			std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd);
			if (cgi == nullptr)
			{
				std::cout << RED << "Cgi is not accessible from singleton\n";
				return;
			}
			response_str = cgi->getRespBody();
			std::cout << GREEN << "RESPONSE\n" << response_str << std::endl << QUIT;

			int act = send(fds_with_flag_[i].pollfd_.fd, response_str.c_str(), response_str.length(), 0);
			CgiSingleton::getInstance().remove_event(fds_with_flag_[i].pollfd_.fd);
			if (act <= 0)
				fds_with_flag_[i].pollfd_.events = POLLERR; // operation failed
			else
			{
				// close(fds_with_flag_[i].pollfd_.fd);
				fds_with_flag_[i].pollfd_.events = POLLHUP; //closing the socket
			}
		}
	}
	else if (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.wasExecuted() == false)
	{
		std::string response_str;

		if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd) == nullptr)
		{
			std::cout << YELLOW << "ADDING EVENT:" << fds_with_flag_[i].pollfd_.fd << QUIT<< std::endl;
			CgiSingleton::getInstance().add_event(fds_with_flag_[i].pollfd_.fd, std::make_shared<Cgi>(fds_with_flag_[i].pollfd_.fd, fds_with_flag_[i].req_));
		}
		std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd);
		if (!cgi->hasForked())
			cgi->execution_close();
		if (cgi->performed_wait())
		{
			fds_with_flag_[i].req_.setExecuted(true);
			response = cgi->response_of_cgi(response);
			response_str = response.respond(fds_with_flag_[i].req_);
			cgi->setResponseBody(response_str);
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
	try
	{
		fds_with_flag_[i].req_.isValid();
	}
	catch(...)
	{
		std::cerr << "Invalid request\n";
		return ;
	}
	
	fds_with_flag_[i].req_.setCurrentServer(fds_with_flag_[i].connected_server_);

	if (!fds_with_flag_[i].req_.isCgi())
		fds_with_flag_[i].pollfd_.events |= POLLOUT;
};

void		Poll::disconecting(size_t& i, std::string str)
{
	size_t fd_of_server = fds_with_flag_[i].connected_fds_[0]; // fds_with_flag --> server info, request, ..., pollfd(fd, event, revent)
	close(fds_with_flag_[i].pollfd_.fd);
	std::cout << BLUE << "Client : " << fds_with_flag_[i].pollfd_.fd << 
	" " << str << "Disconnection from Server " << 
	fd_of_server << QUIT << std::endl;
	fds_with_flag_.erase(fds_with_flag_.begin() + i);
	i--;
}

//===================OUTER HELPER FUNCTIONS ==========================================
int		checkingForSignals(const char *buffer, int bytes, const std::string final_buffer)
{
	(void) final_buffer;
	// if (bytes == 1 && buffer[0] == 0x04)// ev
	// 	return EOF_FLAG;
	for (int i = 0; i < bytes; i++)
		if (((unsigned char)buffer[i] == 0x04))
			return SIG;
	for (int i = 0; i < bytes - 1; ++i)
		if ((unsigned char)buffer[i] == 0xff && (unsigned char)buffer[i + 1] == 0xf4)
			return SIG;
	return NO;		
};

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
		// close(fds_with_flag_[i].pollfd_.fd);
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
	std::string protected_final_buffer;
	while (l != (size_t)bytes)
	{
		if (isprint(((unsigned char)buffer[l])) || (isspace((unsigned char)buffer[l])))
			protected_final_buffer.push_back(buffer[l]);
		l++;
	}
	fds_with_flag_[i].final_buffer_.append(protected_final_buffer);
	return NO;
};
