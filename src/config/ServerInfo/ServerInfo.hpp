#pragma once

#include "../Location/Location.hpp"

struct ServerInfo 
{
	//------------------------Default methods-----------------------------
	ServerInfo();
	ServerInfo(std::filesystem::path absolute_path);
	ServerInfo(const ServerInfo& other);
	ServerInfo& operator=(const ServerInfo& other);
	~ServerInfo();
	//------------------------simple inputs-------------------------------
	int							server_timeout;
	int							listen;
	std::string					server_name;
	std::string					index;
	long long					client_max_body_size;
	//------------------------vector inputs-------------------------------
	std::vector <int>			errors;
	std::vector<Location>		locations;
	//--------------------------paths-------------------------------------
	std::filesystem::path 		executable_folder_server;
	std::filesystem::path		www_path;
	std::filesystem::path		errors_path;
	std::filesystem::path		uploads_dir;
	//--------------------------flags-------------------------------------
	int 						valid_inputs;
	int							valid_errors;
	int							valid_locations;
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
	int							allSimpleInputsValid();
	void						validErrorPath(std::string value);
	void						validErrorType(std::string value);
	int							allErrorsValid();
	void						allLocationValid(std::string value);
	void						allServerInputsValid(std::string value);
};

