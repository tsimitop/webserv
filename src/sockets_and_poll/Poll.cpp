#include "../../inc/sockets_and_poll/Poll.hpp"

#define MAX_LEN 1024

// Helper functions
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
//Default Constructors
Poll::Poll():
poll_success_flag_(YES), 
port_(8081), 
host_name_("localhost"), 
max_body_ln_(1024),
server_timeout_(10000),
max_queued_clients_(5), 
fds_(),
// flaged_fds_(),
server_fd_(0), 
addr_(),
config_()
{};
Poll::Poll(const Poll& other)
{
	poll_success_flag_ = other.poll_success_flag_;
	port_ = other.port_;
	host_name_ = other.host_name_;
	max_body_ln_ = other. max_body_ln_;
	server_timeout_ = other.server_timeout_;
	max_queued_clients_ = other.max_queued_clients_;
	fds_ = other.fds_;
	// flaged_fds_ = other.flaged_fds_;
	server_fd_ = other.server_fd_;
	addr_ = other.addr_;
	config_ = other.config_;
};
Poll::Poll(
				int port, 
				std::string host_name, 
				int max_body_ln, 
				int server_timeout
			):
port_(port),
host_name_(host_name),
max_body_ln_(max_body_ln),
server_timeout_(server_timeout)
{};

Poll& Poll::operator=(const Poll& other)
{
	if (this != &other)
	{
		poll_success_flag_ = other.poll_success_flag_;
		port_ = other.port_;
		host_name_ = other.host_name_;
		max_body_ln_ = other. max_body_ln_;
		server_timeout_ = other.server_timeout_;
		max_queued_clients_ = other.max_queued_clients_;
		fds_ = other.fds_;
		// flaged_fds_ = other.flaged_fds_;
		server_fd_ = other.server_fd_;
		addr_ = other.addr_;
		config_ = other.config_;
	}
	return *this;
};
Poll::~Poll(){};
//setters
void Poll::setConfig(const Http& new_config)
{
	config_ = new_config;
};
//Creating the socket
int Poll::creatingTheServerSocket()
{
	int poll_success_flag_ = YES;
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
			if (getaddrinfo(
							s.server_name_.c_str(), 
							port_char_pointer.c_str(), 
							&temp, 
							&res
							) != 0
				)
			{
				std::cerr << "Error: Get Address Info Failed!\n";
				freeaddrinfo(res);
				poll_success_flag_ = NO;
				continue;
			};
		}
		else
		{
			std::cerr << "The port or host_name is empty!\n";
			poll_success_flag_ = NO;
			continue;
		}
		server_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (server_fd_ == -1)
		{
			std::cerr << "Error: Socket Failed!\n";
			freeaddrinfo(res);
			poll_success_flag_ = NO;
			continue;
		}
		setNonBlockingFd(server_fd_);
		int opt = 1;
		if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			std::cerr << "Error: SockOpt failed!\n";
			freeaddrinfo(res);
			poll_success_flag_ = NO;
			continue;
		};
		if (bind(server_fd_,res->ai_addr,res->ai_addrlen) == -1)
		{
			std::cerr << "Error: Bind failed!\n";
			freeaddrinfo(res);
			poll_success_flag_ = NO;
			continue;
		};
		listen(server_fd_, max_queued_clients_);
		fds_.push_back((pollfd){server_fd_, POLLIN, 0});
		// flaged_fds_.push_back((PollFdWithFlag){{server_fd_, POLLIN, 0}, SERVER});
		std::cout << "Server is listening to the port: " << s.listen_ << "\n";
		// freeing the addrinfo
		freeaddrinfo(res);
	}
	return poll_success_flag_;
};
void Poll::pollingFds()
{
	while(YES)
	{
		int activity = poll(fds_.data(),fds_.size(), config_.servers_[0].server_timeout_); // ?? how I'm taking timeout different per server
		if (activity == -1)
		{
			std::cerr << "Error: Poll failed or Timed out!\n";
			poll_success_flag_ = NO;
			break;
		}
		for (size_t i = 0; i != config_.servers_.size(); i++)
		{
			if (fds_[i].revents & POLLIN)
			{
				int client_fd = accept(fds_[i].fd, NULL, NULL);
				if (client_fd == -1)
					continue ;
				else
				{
					setNonBlockingFd(client_fd);
					fds_.push_back((pollfd){client_fd, POLLIN, 0});
					std::cout << "Client Connected to the FD: " << client_fd << "\n";
				}
			}
		}
		for (size_t i = config_.servers_.size(); i != fds_.size(); i++)
		{
			if (fds_[i].revents & POLLIN)
			{
				char buffer[MAX_LEN]; // how I 'm taking the max body len  per server
				int bytes = recv(fds_[i].fd, buffer, sizeof(buffer), 0);
				HttpRequest req;
				std::string request = buffer;
				std::cout <<GREEN << "request" << request <<QUIT<< std::endl;
				if (bytes <= 0)
				{
					std::cout << "Client with the FD: " << fds_[i].fd << " Disconnected!\n";
					close(fds_[i].fd);
					fds_.erase(fds_.begin() + i);
					--i;
				}
				else
				{
					buffer[bytes] = '\0';
					int steps_back = 0;
					std::string filtered_buffer = filteredBuffer(buffer, steps_back);
					if (bytes - steps_back > 0)
						std::cout << "Client from FD: " << fds_[i].fd << " send the message: " << buffer << "\n";
					
					req.readRequest(request);
					std::vector<ServerInfo>::iterator it;
					for (it = config_.servers_.begin(); it != config_.servers_.end(); it++)
					{
						// std::cout << RED << "SETTING SERVEEEEEEER" << QUIT << std::endl;

						// std::cout << "(*it).listen_: " << (*it).listen_ << "\n";
						// std::cout << "req.getPort(): " << req.getPort() << "\n";
						if (req.getPort() == (*it).listen_)
						{
							// std::cout << BLUE << "SETTING SERVEEEEEEER" << QUIT << std::endl;
							req.setCurrentServer(*it);
						}
					}
					// std::cout << MAGENTA << "req.getCurrentServer().listen_->->-> " << req.getCurrentServer().listen_ << QUIT << std::endl;
					// std::cout << YELLOW << "req.getCurrentServer().listen_->->-> " << req.getCurrentServer().listen_ << QUIT << std::endl;
					HttpResponse response;
					response = req.performMethod();
					std::string resp = response.respond(req);
					std::cout << GREEN << resp << std::endl << QUIT;
					send(fds_[i].fd, resp.c_str(), resp.length(), 0);
				}
			}
		}
	}
	for (size_t i = 0; i != config_.servers_.size(); i++)
		close(fds_[i].fd);
	while (!fds_.empty())
		fds_.erase(fds_.begin());
};