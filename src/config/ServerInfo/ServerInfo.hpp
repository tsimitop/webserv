#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "../Location/Location.hpp"
enum answer{YES = 1, NO = 0};
struct ServerInfo 
{
	//Default
	ServerInfo();
	ServerInfo(std::filesystem::path absolute_path);
	ServerInfo(const ServerInfo& other);
	ServerInfo& operator=(const ServerInfo& other);
	~ServerInfo();

	int							server_timeout;
	int							listen;
	std::string					server_name;
	std::string					index;
	long long					client_max_body_size;
	std::vector <int>			errors;
	std::vector<Location>		locations;
	//--------------------------paths-------------------------------------
	std::filesystem::path 		executable_path;
	std::filesystem::path		www_path;
	std::filesystem::path		errors_path;
	std::filesystem::path		uploads_path;
	//--------------------------flags-------------------------------------
	int 						valid_inputs;
	//methods
	//helpers
	std::string 				spaceTrimmer(std::string str);
	size_t 						calcBodySizeInBites(size_t body_size);
	//validators	
	void						validServerTimeOut(std::string value);
	void						validListen(std::string value);
	void						validServerName(std::string value);
	void						validIndex(std::string value);
	void						validClientMaxBodySize(std::string value);
	void						allSimpleInputsValid(std::string value);
	void						validErrorType(std::string value);
	void						validErrorPath(std::string value);
	void						allErrorsValid(std::string value);
	void						validLocation(std::string value);
	void						allLocationValid(std::string value);
	void						allServerInputsValid(std::string value);
};

int								strIsNumber(std::string str);
int								strIsAlphaOr(std::string str, char extraChar);