#pragma once

#include "./PollFdWithFlag.hpp"
#include "../cgis/CgiSingleton.hpp"
#include <memory>

struct Poll 
{
	//===============ATTRIBUTES =====================================================
	int 		poll_success_flag_;
	int 		max_queued_clients_;
	std::vector<pollfd> fds_;
	std::vector<PollFdWithFlag> fds_with_flag_;
	std::vector<ServerInfo> active_servers;
	int 		server_fd_;
	sockaddr_in addr_;
	Http		config_;
	size_t		number_of_config_valid_servers_;
	
	//===============DEFAULTS========================================================
	Poll();
	Poll(const Poll& other);
	// Poll(Poll&& other) noexcept = default;
	// Poll& operator=(Poll&& other) noexcept = default;
	Poll& operator=(const Poll& other);
	~Poll();
	//===============SETTERS=========================================================
	void setConfig(const Http& new_config);
	//===============BINDING && SYNCHRONUS I/O ======================================
	int 		binding();
	void 		synchroIO();
	//===============POLL CALL ======================================================
	int			polling();
	//===============POLL STATES ====================================================
	void		pollhup(size_t& i);
	int			pollin(size_t i);
	void		pollout(size_t i);
	//================HELPER METHODS ================================================
	void		connecting();
	void		disconecting(size_t& i, std::string str);
	void		closingServers();
	void		setMaxBodyLen(size_t i, int bytes);
	void		definingRequest(size_t i);
	size_t		lengthProt(size_t i);
	int			eAgainAndEWouldblock(size_t i, int bytes);
	int			updateFinalBuffer(size_t i, int bytes, char buffer[]);
	void		validPollout();
	void		nonValidSigletonCase(size_t i, std::string& response_str);
	void		validSigletonCase(size_t i, HttpResponse &response);
};

//===================OUTER HELPER FUNCTIONS ==========================================
int 			setNonBlockingFd(int fd);
std::string		filteredBuffer(std::string buffer, int& steps_back);
std::string 	safeNullTermination(int bytes, size_t max_body_ln, std::string buffer);
int				checkingForSignals(const char *buffer, int bytes, const std::string final_buffer);






// void		Poll::pollout(size_t i)
// {
// 	HttpResponse response;
// 	if(fds_with_flag_[i].pollfd_.events & POLLOUT)
// 	{
// 		std::string response_str;
// 		if (!fds_with_flag_[i].req_.isCgi() \
// 		 || (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.isInvalid()) \
// 		 || (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.isForbidden()))
// 		{
// 			response = fds_with_flag_[i].req_.performMethod();
// 			response_str = response.respond(fds_with_flag_[i].req_);
// 			int act = send(fds_with_flag_[i].pollfd_.fd, response_str.c_str(), response_str.length(), 0);
// 			if (act < 0)
// 				fds_with_flag_[i].pollfd_.events = POLLERR; // operation failed
// 			else
// 				fds_with_flag_[i].pollfd_.events = POLLHUP ; //closing the socket
// 		}
// 		else
// 			nonValidSigletonCase(i ,response_str);
// 	}
// 	else if (fds_with_flag_[i].req_.isCgi() && fds_with_flag_[i].req_.wasExecuted() == false)
// 		validSigletonCase(i ,response);
// };
// void Poll::validSigletonCase(size_t i, HttpResponse &response)
// {
// 	std::string response_str;
// 	if (CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd) == nullptr)
// 	{
// 		std::cout << YELLOW << "ADDING EVENT:" << fds_with_flag_[i].pollfd_.fd << QUIT<< std::endl;
// 		CgiSingleton::getInstance().add_event(fds_with_flag_[i].pollfd_.fd, std::make_shared<Cgi>(fds_with_flag_[i].pollfd_.fd, fds_with_flag_[i].req_));
// 	}
// 	std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd);
// 	if (!cgi->hasForked())
// 		cgi->execution_close();
// 	if (cgi->performed_wait())
// 	{
// 		fds_with_flag_[i].req_.setExecuted(true);
// 		response = cgi->response_of_cgi(response);
// 		response_str = response.respond(fds_with_flag_[i].req_);
// 		cgi->setResponseBody(response_str);
// 	}
// }

// void Poll::nonValidSigletonCase(size_t i, std::string& response_str)
// {
// 	std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(fds_with_flag_[i].pollfd_.fd);
// 	if (cgi == nullptr)
// 	{
// 		std::cout << RED << "Cgi is not accessible from singleton\n";
// 		return;
// 	}
// 	response_str = cgi->getRespBody();
// 	std::cout << GREEN << "RESPONSE\n" << response_str << std::endl << QUIT;

// 	int act = send(fds_with_flag_[i].pollfd_.fd, response_str.c_str(), response_str.length(), 0);
// 	CgiSingleton::getInstance().remove_event(fds_with_flag_[i].pollfd_.fd);
// 	if (act <= 0)
// 		fds_with_flag_[i].pollfd_.events = POLLERR; // operation failed
// 	else
// 		fds_with_flag_[i].pollfd_.events = POLLHUP; //closing the socket
// }