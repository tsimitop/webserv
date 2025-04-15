#pragma once

#include "Location.hpp"

struct ServerInfo 
{
	//------------------------Default methods-----------------------------
	ServerInfo();
	ServerInfo(std::filesystem::path absolute_path);
	ServerInfo(const ServerInfo& other);
	ServerInfo& operator=(const ServerInfo& other);
	~ServerInfo();
	//------------------------lines of the server-------------------------
	std::vector<std::string>				lines_of_server_;
	std::vector<size_t>						location_indexes_;
	//------------------------simple inputs-------------------------------
	int										keep_alive_timeout_;
	int										send_timeout_;
	int										server_timeout_;
	int										listen_;
	std::string								server_name_;
	std::string								index;
	long long								client_max_body_size_; // * 1024 value
	//------------------------vector inputs-------------------------------
	std::map <int, std::filesystem::path>	errors; // error type  and error path
	std::vector<Location>					locations_;
	//--------------------------paths-------------------------------------
	std::filesystem::path 					executable_root_server_;
	std::filesystem::path					www_path_;
	std::filesystem::path					errors_path_;
	std::filesystem::path					uploads_dir_;
	//--------------------------flags-------------------------------------
	int 									valid_inputs_;
	//methods
	//helpers
	std::string 							spaceTrimmer(std::string str);
	size_t 									calcBodySizeInBites(size_t body_size);
	//validators
	//--------------------------Server--------------------------------
	void									validServerTimeOut(std::string value);
	void									validListen(std::string value);
	void									validServerName(std::string value);
	void									validIndex(std::string value);
	void									validClientMaxBodySize(std::string& value);
	int										allSimpleInputsValid();
	void									validErrorPath(std::string value);
	void									validErrorType(std::string value);
	void									defaultErrorSetting();
	//setting the values
	void									setServerTimeOut(std::string line, int& attribute);
	void									setListen(std::string line);
	void									setServerName(std::string line);
	void									setIndex(std::string line);
	void									setClientMaxBodySize(std::string line);
	void									pushToErrors(std::string line);
	void									locationIndexes();
	void									pushLocationsLines();
	void									parsingLocations();
};
//making the parcing for the config and have some basic undestanding about config

// server{listen=4243;max_body_client_size=10m;location/updates{index=index.html;root=./updates};location/updates{index=index.html;root=./updates}location/updates{index=index.html;root=./updates}};
// server{listen=4243;max_body_client_size=10m;location/updates{index=index.html;root=./updates};};
// server{listen=4243;max_body_client_size=10m;location/updates{index=index.html;root=./updates}};

