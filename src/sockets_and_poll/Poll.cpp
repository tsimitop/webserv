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
				0, 0, "", 0,
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
				continue ;
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
					0, 0, "", 0,
					{(size_t)fds_with_flag_[i].pollfd_.fd}, config_.active_servers_[i]}); // saving with which port they are connected and in which server
					std::string uploads_dir_str = fds_with_flag_[i].connected_server_.uploads_dir_.string();
				std::cout <<YELLOW  << "fds_with_flag["<< fds_with_flag_.size() - 1<< "] | Client: " << client_fd << " connected to fd[" << fds_with_flag_[i].pollfd_.fd << "] " 
				<< "| with server_listen : " << fds_with_flag_[i].connected_server_.listen_ << " | server_root: " << fds_with_flag_[i].connected_server_.root_ 
				<< " | uploads_dir: " << uploads_dir_str.substr(uploads_dir_str.find_last_of("/") + 1)<<  QUIT<< std::endl;
			}
			fds_with_flag_[i].connected_fds_.push_back((size_t)client_fd);
		}
	}
};
void Poll::synchroIO()
{
	while(poll_flag == YES)
	{
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd) && CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd)->cgiPidDone())
					fds_with_flag_[i].pollfd_.events |= POLLOUT;

		}
		int activity = polling();
		if (activity == -1)
		{
			// if (errno == EINVAL)
			// 	std::cerr<< RED << "\nError: poll failed!File dirrectories exceeding the max fds or wrong time out value!" << QUIT << std::endl;
			// if (errno == ENOMEM)
			// 	std::cerr << RED << "\nError: poll failed! Not allocation enough space!" << QUIT << std::endl;
			continue;
		}
		connecting();
		for (size_t i = config_.active_servers_.size(); i != fds_with_flag_.size(); i++)
		{
			pollhup(i);
			int pollin_int = pollin(i);
			if (pollin_int == NO)
				continue;
			pollout(i);
		}
	}
	if (poll_flag == NO)
		closingServers();
};
//===============POLL STATES ====================================================
void		Poll::pollhup(size_t& i)
{
	std::string poll_err = fds_with_flag_[i].pollfd_.revents & POLLERR ? "POLLERR:" :
							fds_with_flag_[i].pollfd_.revents & POLLHUP ?  "POLLHUP: " :
							fds_with_flag_[i].pollfd_.revents & POLLNVAL ?  "POLLNVAL: " :
							fds_with_flag_[i].pollfd_.revents &  POLLWRBAND ? " POLLWRBAND: ":
							fds_with_flag_[i].pollfd_.revents & POLLRDNORM ? " POLLRDNORM" :
							"UNCLASSIFIED POLL HUNG UP: ";
	if (fds_with_flag_[i].pollfd_.revents & (POLLERR | POLLHUP | POLLNVAL | POLLWRBAND | POLLRDNORM)) // revent = POLLERR & POLLER != 0 || 1 0 0 0 0 , 0 1 0 0 0 0 0
		disconecting(i, poll_err);
};
int	Poll::pollin(size_t i)
{
	int answer = YES;
	if (fds_with_flag_[i].pollfd_.revents & (POLLIN))
	{
		size_t temp_len = lengthProt(i); // comment out in case of compilation failure
		char buffer[temp_len]; // maybe replace temp_len occurences with 100000 in case of compilation failure
		memset(buffer, 0, temp_len); //
		int bytes = recv(fds_with_flag_[i].pollfd_.fd, buffer, temp_len, 0);
		// std::cout << MAGENTA << buffer << QUIT << std::endl;
		if (bytes == 0 || bytes < 0)
			answer = eAgainAndEWouldblockForReq(i, bytes);
		else
		{
			if ((size_t)bytes < temp_len)
				buffer[bytes] = '\0';
			std::string	temp_buffer = buffer;
			size_t final_buffer_pre_append_last_element_index;
			if(!temp_buffer.empty() && temp_buffer[0] != '\0')
				final_buffer_pre_append_last_element_index= fds_with_flag_[i].final_buffer_.size();
			fds_with_flag_[i].final_buffer_.append(temp_buffer);
			bool	rnrn_found = fds_with_flag_[i].final_buffer_.find("\r\n\r\n") != std::string::npos;
			bool	is_post = fds_with_flag_[i].final_buffer_.find("POST ", 0, 4) != std::string::npos;
			if (rnrn_found == NO)
			{
				fds_with_flag_[i].pollfd_.events = POLLIN;
				return NO;
			}
			else
			{
				if (is_post == NO)
				{
					definingRequest(i);
					fds_with_flag_[i].setFinalRespBuffer();
					fds_with_flag_[i].final_buffer_.push_back('\0');
					return YES;
				}
				else
				{
					std::string body_of_post;
					fds_with_flag_[i].req_.readRequest(fds_with_flag_[i].final_buffer_, 1);
					if (fds_with_flag_[i].req_.isValid())
					{
						fds_with_flag_[i].setFileType();
						body_of_post = fds_with_flag_[i].req_.getBody();
						bool is_accepted_file =fds_with_flag_[i].file_type_ == "txt" || fds_with_flag_[i].file_type_ == "md" || fds_with_flag_[i].req_.isCgi();
						bool is_accepted_length = (size_t)fds_with_flag_[i].content_length_ <= (size_t)fds_with_flag_[i].connected_server_.locations_[0].client_max_body_size_;
						if (!is_accepted_file || !is_accepted_length)
						{
							definingRequest(i);
							fds_with_flag_[i].setFinalRespBuffer();
							return YES;
						}
					}
					if ((body_of_post.length() < fds_with_flag_[i].content_length_ ))
						{
							fds_with_flag_[i].pollfd_.events = POLLIN;
							return NO;
						}
					else
					{
						if (fds_with_flag_[i].content_length_ != 0)
							fds_with_flag_[i].final_buffer_[final_buffer_pre_append_last_element_index + bytes] = '\0';
						definingRequest(i);
						fds_with_flag_[i].setFinalRespBuffer();
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
	if(fds_with_flag_[i].pollfd_.revents & POLLOUT)
	{
		bool is_cgi = fds_with_flag_[i].req_.isCgi();
		bool is_valid_cgi = is_cgi
		&& !(is_cgi && fds_with_flag_[i].req_.isInvalid())
		&& !(is_cgi && fds_with_flag_[i].req_.isForbidden());
		bool is_agent = YES;
		if (!fds_with_flag_[i].final_buffer_.empty())
			is_agent = (fds_with_flag_[i].final_buffer_.find("User-Agent: ") != std::string::npos);
		int act = send(fds_with_flag_[i].pollfd_.fd, fds_with_flag_[i].final_resp_buffer_.c_str(), fds_with_flag_[i].final_resp_buffer_.length(), MSG_NOSIGNAL);
		if (is_valid_cgi)
			CgiSingleton::getInstance().remove_event(fds_with_flag_[i].pollfd_.fd);
		if (act < 0)
			eAgainAndEWouldblockForResp(i, act);
		else
		{
			#if defined(__APPLE__)
				if (fds_with_flag_[i].final_buffer_.substr(0,4) != "POST" || is_cgi || !is_agent)
					fds_with_flag_[i].pollfd_.events = POLLHUP;
			#else
				if ((fds_with_flag_[i].final_buffer_.substr(0,4) != "POST" && is_cgi) || !is_agent)
					fds_with_flag_[i].pollfd_.events = POLLHUP;
				else
					fds_with_flag_[i].pollfd_.events |= POLLHUP;
			#endif
		}
	}
	else if (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.wasExecuted() == false)
	{
		fds_with_flag_[i].setFinalRespBufferIfCgi();
	}
	if (fds_with_flag_[i].method_is_finished_ == YES)
	{
		fds_with_flag_[i].final_buffer_.clear();
		fds_with_flag_[i].final_resp_buffer_.clear();
	}
};
//================HELPER METHODS ================================================
void		Poll::closingServers()
{
	if (!fds_with_flag_.empty())
	{
		std::cout <<  BLUE << "Closing servers\n" << QUIT;
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
	(void)i;
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
	std::cout << BLUE << "Client : " << fds_with_flag_[i].pollfd_.fd << 
	" " << str << "Disconnection from Server " << 
	fd_of_server << QUIT << std::endl;
	close(fds_with_flag_[i].pollfd_.fd);
	fds_with_flag_.erase(fds_with_flag_.begin() + i);
	i--;
}

//===================OUTER HELPER FUNCTIONS ==========================================

int setNonBlockingFd(int fd)
{
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
		fds_with_flag_[i].pollfd_.events |= POLLOUT;
		return YES;
	}
	else
	{
		close(fds_with_flag_[i].pollfd_.fd);
		fds_with_flag_[i].pollfd_.events |= POLLOUT;
		return YES;
	}
	return NO;
};
void	Poll::eAgainAndEWouldblockForResp(size_t i, int act)
{
	if (act == 0)
	{
		close(fds_with_flag_[i].pollfd_.fd);
		fds_with_flag_[i].pollfd_.events |= POLLHUP;
		return ;
	}
	else
	{
		close(fds_with_flag_[i].pollfd_.fd);
		fds_with_flag_[i].pollfd_.events = POLLERR;
		return ;
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
