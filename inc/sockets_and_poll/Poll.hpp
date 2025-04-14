#pragma once

#include "./PollFdWithFlag.hpp"

struct Poll 
{
	//attributes
	int 		poll_success_flag_;
	int 		port_;
	std::string host_name_;
	size_t 		max_body_ln_;
	int			server_timeout_;
	int 		max_queued_clients_;
	std::vector<pollfd> fds_;
	int 		server_fd_;
	sockaddr_in addr_;
	Http		config_;
	
	//Defaults
	Poll();
	Poll(const Poll& other);
	Poll(int port, std::string host_name, int max_body_ln, int server_timeout);
	Poll(const Http& new_config);
	Poll& operator=(const Poll& other);
	~Poll();
	//sockets && poll
	int 		creatingTheServerSocket();
	void 		pollingFds();
	// for every server I need to go and make a socket with the accept
	// I need to be able to open all the ports
	// So let's try to start from the ports one by one transelating them in working servers
	// from config to addrinfo  host port max_body_size server_timeout
};

// Helper functions
int 			setNonBlockingFd(int fd);
std::string		filteredBuffer(std::string buffer, int& steps_back);
std::string 	safeNullTermination(int bytes, size_t max_body_ln, std::string buffer);
// for every server we will need one port that is running




// #pragma once

// #include "./PollFdWithFlag.hpp"

// struct Poll 
// {
// 	//attributes
// 	//flags
// 	int 		poll_success_flag_;
// 	int 		port_;
// 	std::string host_name_;
// 	size_t 		max_body_ln_;
// 	int			server_timeout_;
// 	int 		max_queued_clients_;
// 	std::vector<pollfd> fds_;
// 	//PollFdWithFlag
// 	// std::vector<PollFdWithFlag> flaged_fds_;
// 	int 		server_fd_;
// 	sockaddr_in addr_;
// 	Http		config_;
	
// 	//Defaults
// 	Poll();
// 	Poll(const Poll& other);
// 	Poll(
// 			int port, 
// 			std::string host_name, 
// 			int max_body_ln, 
// 			int server_timeout
// 		);
// 	Poll(const Http& new_config);
// 	Poll& operator=(const Poll& other);
// 	~Poll();
// 	//sockets && poll
// 	int 		creatingTheServerSocket();
// 	void 		pollingFds();
// 	// for every server I need to go and make a socket with the accept
// 	// I need to be able to open all the ports
// 	// So let's try to start from the ports one by one transelating them in working servers
// 	// from config to addrinfo  host port max_body_size server_timeout
// };

// // Helper functions
// int 			setNonBlockingFd(int fd);
// std::string		filteredBuffer(std::string buffer, int& steps_back);
// std::string 	safeNullTermination(int bytes, size_t max_body_ln, std::string buffer);
// // for every server we will need one port that is running