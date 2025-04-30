#include "../../inc/sockets_and_poll/Poll.hpp"

#define MAX_BUFFER 4096
#define DOCKER 0

//===============DEFAULTS========================================================
Poll::Poll():
poll_success_flag_(YES), 
max_queued_clients_(128), 
fds_(),
fds_with_flag_(),
server_fd_(0), 
addr_(),
config_(),
number_of_config_valid_servers_(0)
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
	number_of_config_valid_servers_ = other.number_of_config_valid_servers_;
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
		number_of_config_valid_servers_ = other.number_of_config_valid_servers_;
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
	for (ServerInfo& s : config_.config_valid_servers_)
	{
			addrinfo temp, *res = nullptr;
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
					s.valid_server_ = NO;
					freeaddrinfo(res);
					continue;
				};
			}
			else
			{
				std::cerr << RED << "The port or host_name is empty!" << QUIT<<std::endl;
				s.valid_server_ = NO;
				continue;
			}
			server_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (server_fd_ == -1)
			{
				std::cerr << RED << "Error: Socket Failed!" << QUIT<<std::endl;
				s.valid_server_ = NO;
				freeaddrinfo(res);
				continue;
			}
			setNonBlockingFd(server_fd_);
			int opt = 1;
			if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			{
				std::cerr << RED << "Error: SockOpt failed!" << QUIT<<std::endl;
				s.valid_server_ = NO;
				freeaddrinfo(res);
				continue;
			};
			if (bind(server_fd_,res->ai_addr,res->ai_addrlen) == -1)
			{
				std::cerr << RED << "Error: Bind failed!" << QUIT<<std::endl;
				s.valid_server_ = NO;
				freeaddrinfo(res);
				continue;
			};
			listen(server_fd_, max_queued_clients_);
			config_.active_servers_.push_back(s);
			fds_with_flag_.push_back(
				(PollFdWithFlag){{server_fd_, POLLIN, 0}, 
				FIRST_TIME, SERVER, NO,
				{}, {},
				(HttpRequest){"", s},
				(HttpResponse){},
				0, 0, 0,
				{}, s});
			std::string uploads_dir_str_version = s.locations_[0].uploads_dir_.string();
			std::cout << CYAN <<
				"Server: fd[" << server_fd_ <<
				"] | port: " << s.listen_ <<
				" | root: " << s.root_ <<
				" | uploads_dir: "<< uploads_dir_str_version.substr(uploads_dir_str_version.find_last_of("/") + 1) << 
				QUIT << std::endl;
			freeaddrinfo(res);
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
			{
				std::cout << "client_fd == -1!!!!!!!\n";
				continue ;
			}
			else
			{
				setNonBlockingFd(client_fd);
				fds_with_flag_.push_back(
					(PollFdWithFlag){(pollfd){client_fd, POLLIN, 0}, 
					FIRST_TIME,
					CLIENT,
					NO,
					{}, {},
					(HttpRequest){"", config_.active_servers_[i]},
					(HttpResponse){},
					0, 0, 0,
					{(size_t)fds_with_flag_[i].pollfd_.fd}, config_.active_servers_[i]}); // saving with which port they are connected and in which server
					std::string uploads_dir_str = fds_with_flag_[i].connected_server_.uploads_dir_.string();
				std::cout <<YELLOW  << "fds_with_flag["<< fds_with_flag_.size() - 1<< "] | Client: " << client_fd << " connected to fd[" << fds_with_flag_[i].pollfd_.fd << "] " 
				<< "| with server_listen : " << fds_with_flag_[i].connected_server_.listen_ << " | server_root: " << fds_with_flag_[i].connected_server_.root_ 
				<< " | uploads_dir: " << uploads_dir_str.substr(uploads_dir_str.find_last_of("/") + 1)<<  QUIT<< std::endl;
			}
			fds_with_flag_[i].connected_fds_.push_back((size_t)client_fd);
			// return;
		}
	}
};
void Poll::synchroIO()
{
	while(poll_flag == YES)
	{
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			// if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd))
			// 		std::cout << (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd)->cgiPidDone()) << " = CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd)->cgiPidDone()\n";
			if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd) && CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd)->cgiPidDone())
					fds_with_flag_[i].pollfd_.events |= POLLOUT;

		}
		int activity = polling();
		if (activity == -1)
		{
			// if (errno == EINTR)
			// 	std::cerr<< RED << "\nError: poll failed! Interupted signal!" << QUIT << std::endl;
			if (errno == EINVAL)
				std::cerr<< RED << "\nError: poll failed!File dirrectories exceeding the max fds or wrong time out value!" << QUIT << std::endl;
			if (errno == ENOMEM)
				std::cerr << RED << "\nError: poll failed! Not allocation enough space!" << QUIT << std::endl;
			break;
		}
		connecting();
		if (activity == 0)
			disconnectingPreviousSuccededMethods("POLLSP: ");
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			int er = errno;
			(void)er;
			pollhup(i);
			int pollin_int = pollin(i);
			if (pollin_int == NO)
				continue;
			else if (pollin_int == SIG)
			{
				fds_with_flag_[i].pollfd_.events = POLLHUP | POLLERR;
			}
			pollout(i);
		}
	}
	if (poll_flag == NO)
		closingServers();
};
void		Poll::disconnectingPreviousSuccededMethods(std::string str)
{
	if (config_.active_servers_.size() + 1 < fds_with_flag_.size())
	{
		for (size_t i = config_.active_servers_.size(); i + 1!= fds_with_flag_.size(); i++)
		{
			if (fds_with_flag_[i].method_is_finished_ == YES)
			{
				for(size_t k = i + 1; k != fds_with_flag_.size(); k++)
				{
					if(
						fds_with_flag_[i].connected_server_.listen_ == fds_with_flag_[i].connected_server_.listen_ && 
						fds_with_flag_[i].method_is_finished_ == fds_with_flag_[k].method_is_finished_ 
					)
					{
						std::cout << "About to disconnect successful method\n";
						disconecting(i, str);
						break ;
					}
				}
			}
		}
	}
}
//===============POLL STATES ====================================================
void		Poll::pollhup(size_t& i)
{
	// std::cout << GREEN << "ENTERED POLLHUP FUNCTION\n";
	std::string poll_err = fds_with_flag_[i].pollfd_.revents & POLLERR ? "POLLERR:" :
							fds_with_flag_[i].pollfd_.revents & POLLHUP ?  "POLLHUP: " :
							fds_with_flag_[i].pollfd_.revents & POLLNVAL ?  "POLLNVAL: " :
							fds_with_flag_[i].pollfd_.revents &  POLLWRBAND ? " POLLWRBAND: ":
							fds_with_flag_[i].pollfd_.revents & POLLRDNORM ? " POLLRDNORM" :
							"UNCLASSIFIED POLL HUNG UP: ";
	if (fds_with_flag_[i].pollfd_.revents & (POLLERR | POLLHUP | POLLNVAL | POLLWRBAND | POLLRDNORM)) // revent = POLLERR & POLLER != 0 || 1 0 0 0 0 , 0 1 0 0 0 0 0
	{
		std::cout << "About to disconnect in POLLHUP\n";
		disconecting(i, poll_err);
	}
};

int	Poll::pollin(size_t i)
{
	int answer = YES;
	if (fds_with_flag_[i].pollfd_.revents & (POLLIN))
	{
		size_t temp_len = lengthProt(i);
		char buffer[temp_len];
		memset(buffer, 0, temp_len); //
		int bytes = recv(fds_with_flag_[i].pollfd_.fd, buffer, temp_len, 0);
		std::cout << MAGENTA << buffer << QUIT << std::endl;
		if (bytes == 0 || bytes < 0)
			answer = eAgainAndEWouldblockForReq(i, bytes);
		else
		{
			std::cout << "ELSE000\n";
			buffer[bytes] = '\0';
			std::cout << "kollisa00\n";
			// int checking_signals = checkingForSignals(buffer, bytes, fds_with_flag_[i].final_buffer_);
			// std::cout << SIG << " = SIG\t" << checking_signals << " = checking_signals\n";
			// if (checking_signals == SIG)
			// 	return SIG;
			std::cout << "kollisa\n";

			std::string	temp_buffer = buffer;
			fds_with_flag_[i].final_buffer_.append(temp_buffer);
			bool	rnrn_found = fds_with_flag_[i].final_buffer_.find("\r\n\r\n") != std::string::npos;
			bool	is_post = fds_with_flag_[i].final_buffer_.find("POST ", 0, 4) != std::string::npos;
			std::cout << "before if\n";
			if (rnrn_found == NO)
			{
				std::cout << "rnrn_found == NO\n";

				return NO;
			}
			else
			{
				std::cout << "ELSE111\n";
				if (is_post == NO)
				{
					definingRequest(i);
					fds_with_flag_[i].final_buffer_.push_back('\0');
					return YES;
				}
				else
				{
					size_t start = fds_with_flag_[i].final_buffer_.find("Content-Length: ") + 16;
					size_t end = fds_with_flag_[i].final_buffer_.substr(start).find("\r\n") + start;
					size_t content_length = std::stol(fds_with_flag_[i].final_buffer_.substr(start, end));
					std::string body_of_post = fds_with_flag_[i].final_buffer_.substr(fds_with_flag_[i].final_buffer_.find("\r\n\r\n") + 4);
					size_t start_b = fds_with_flag_[i].final_buffer_.find("filename=") + 10;
					size_t end_b = fds_with_flag_[i].final_buffer_.substr(start_b).find_first_of("\"");
					std::string filename = fds_with_flag_[i].final_buffer_.substr(start_b, end_b);
					bool apparent_filetype = filename.find_last_of(".") != std::string::npos && filename.find_last_of(".") == filename.find_first_of(".");
					std::string filetype;
					if (apparent_filetype == true)
						filetype = filename.substr(apparent_filetype, end_b);
					if (!filetype.empty())
					{
						if (filetype != "txt" && filetype != "md")
						{
							definingRequest(i);
							return YES;
						}
					}
					std::cout << filename << " = filename\n";

					if (body_of_post.length() < content_length)
						return NO;
					else
					{
						definingRequest(i);
						fds_with_flag_[i].final_buffer_.push_back('\0');
						return YES;
					}
				}
			}
		}
	}
	return answer;
};
void		Poll::pollout(size_t i)
{
	HttpResponse response;

	bool is_cgi = fds_with_flag_[i].req_.isCgi();
	bool is_valid_cgi = is_cgi
	&& !(is_cgi && fds_with_flag_[i].req_.isInvalid())
	&& !(is_cgi && fds_with_flag_[i].req_.isForbidden());

	if(fds_with_flag_[i].pollfd_.revents & POLLOUT)
	{
		if (fds_with_flag_[i].final_resp_buffer_.empty())
			fds_with_flag_[i].setFinalRespBuffer();

		int act = send(fds_with_flag_[i].pollfd_.fd, fds_with_flag_[i].final_resp_buffer_.c_str(), fds_with_flag_[i].final_resp_buffer_.length(), 0);
		if (is_valid_cgi)
			CgiSingleton::getInstance().remove_event(fds_with_flag_[i].pollfd_.fd);
		if (act <= 0)
			eAgainAndEWouldblockForResp(i, act);
		else
		{

			// size_t chunk_length = lengthProt(i);
			if (fds_with_flag_[i].req_.getMethod() != "POST" || is_cgi == YES )
			// || std::stoi(fds_with_flag_[i].req_.getContentLength()) < chunk_length)
				fds_with_flag_[i].pollfd_.events = POLLHUP;
			fds_with_flag_[i].method_is_finished_ = YES;
		}
	}
	else if (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.wasExecuted() == false)
	{
		fds_with_flag_[i].setFinalRespBufferIfCgi();
	}
	if (fds_with_flag_[i].pollfd_.events & (POLLHUP | POLLERR))
	{
		fds_with_flag_[i].final_buffer_.clear();
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
	size_t chunk_size;
	size_t location_max_size = (size_t)fds_with_flag_[i].connected_server_.locations_[0].client_max_body_size_;
	if (location_max_size > 2000000)
		chunk_size = 2000000;
	else if (location_max_size > 1000000)
	{
		chunk_size = 1000000;
	}
	else if (location_max_size > 500000)
		chunk_size = 500000;
	else
		chunk_size
			= location_max_size - 100;
	// if (chunk_size< 1024)
	// 	chunk_size= 1025;
	// if ((size_t)chunk_size> (size_t)fds_with_flag_[i].connected_server_.client_max_body_size_)
	// {
	// 	fds_with_flag_[i].connected_server_.locations_[0].client_max_body_size_ 
	// 	= fds_with_flag_[i].connected_server_.client_max_body_size_ - 1;
	// }
	return (chunk_size);
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
	fds_with_flag_[i].req_.readRequest(fds_with_flag_[i].final_buffer_, 1);
	fds_with_flag_[i].req_.setCurrentServer(fds_with_flag_[i].connected_server_);
	fds_with_flag_[i].req_.isValid();
	if (!fds_with_flag_[i].req_.isCgi())
		fds_with_flag_[i].pollfd_.events |= POLLOUT;
};

void		Poll::disconecting(size_t& i, std::string str)
{
	size_t fd_of_server = fds_with_flag_[i].connected_fds_[0];
	//---- additional for error handling--------------------
	if (fds_with_flag_[i].req_.isInvalid() == NO)
		close(fds_with_flag_[i].pollfd_.fd);
	//---- additional for error handling--------------------
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
	std::cout << "Caught CTRL+D\n";
	for (int i = 0; i < bytes; i++)
		if (((unsigned char)buffer[i] == 0x04))
			return SIG;
	std::cout << "Caught CTRL+C\n";
	for (int i = 0; i < bytes - 1; ++i)
		if ((unsigned char)buffer[i] == 0xff && (unsigned char)buffer[i + 1] == 0xf4)
			return SIG;
	std::cout << "NO SIGNAL\n";
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


int			Poll::eAgainAndEWouldblockForReq(size_t i, int bytes)
{
	if (bytes == 0)
	{
		std::cout << "This string is empty!\n";
		// close(fds_with_flag_[i].pollfd_.fd);
		fds_with_flag_[i].pollfd_.events |= POLLOUT;
		return YES;
		// disconecting(i, "(EOF)");
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return NO;
		else if (errno == EINVAL)
		{
			close(fds_with_flag_[i].pollfd_.fd);
			fds_with_flag_[i].pollfd_.events |= POLLOUT;
			return YES;
		}
	}
			// disconecting(i, "(recv)"); // EINVAL ECONNECTRESET ENOTCONN
	return NO;
};
void	Poll::eAgainAndEWouldblockForResp(size_t i, int act)
{
	if (act == 0)
			{
				std::cout << "This string is empty!\n";
				close(fds_with_flag_[i].pollfd_.fd);
				fds_with_flag_[i].pollfd_.events |= POLLHUP;
				return ;
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					fds_with_flag_[i].pollfd_.events |= POLLOUT;
					return ;
				}
				else if (errno == EINVAL)
				{
					close(fds_with_flag_[i].pollfd_.fd);
					fds_with_flag_[i].pollfd_.events = POLLERR;
					return ;
				}
			}
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
	int just_for_filtered = 0;
	filteredBuffer(protected_final_buffer, just_for_filtered);

	fds_with_flag_[i].final_buffer_.append(protected_final_buffer);
	return NO;
};
