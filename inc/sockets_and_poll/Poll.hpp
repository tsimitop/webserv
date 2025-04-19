#pragma once

#include "./PollFdWithFlag.hpp"

struct Poll 
{
	//===============ATTRIBUTES =====================================================
	int 		poll_success_flag_;
	int 		max_queued_clients_;
	std::vector<pollfd> fds_;
	std::vector<PollFdWithFlag> fds_with_flag_;
	int 		server_fd_;
	sockaddr_in addr_;
	Http		config_;
	
	//===============DEFAULTS========================================================
	Poll();
	Poll(const Poll& other);
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
};

//===================OUTER HELPER FUNCTIONS ==========================================
int 			setNonBlockingFd(int fd);
std::string		filteredBuffer(std::string buffer, int& steps_back);
std::string 	safeNullTermination(int bytes, size_t max_body_ln, std::string buffer);
