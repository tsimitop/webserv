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
	void		disconnectingPreviousSuccededMethods(std::string str);
	void		closingServers();
	void		setMaxBodyLen(size_t i, int bytes);
	void		definingRequest(size_t i);
	size_t		lengthProt(size_t i);
	int			eAgainAndEWouldblockForReq(size_t i, int bytes);
	void		eAgainAndEWouldblockForResp(size_t i, int act);
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