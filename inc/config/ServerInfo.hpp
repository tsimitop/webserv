#pragma once

#include "Location.hpp"

struct ServerInfo 
{
	//===============ATTRIBUTES======================================================
	//-------------lines of the server---------
	std::vector<std::string>				lines_of_server_;
	std::vector<size_t>						location_indexes_;
	//-------------simple inputs---------------
	int										keep_alive_timeout_;
	int										send_timeout_;
	int										server_timeout_;
	int										listen_;
	std::string								root_;
	std::string								server_name_;
	std::string								index;
	long long								client_max_body_size_;

	//-------------vector inputs---------------
	std::map <int, std::filesystem::path>	errors;
	std::vector<Location>					locations_;
	//-------------paths-----------------------
	std::filesystem::path 					executable_root_server_;
	std::filesystem::path					www_path_;
	std::filesystem::path					errors_path_;
	std::filesystem::path					uploads_dir_;
	//-------------flags-----------------------
	int 									valid_server_;
	int										before_locations_;
	//=============DEFAULT CONTRUCTORS ============================================
	ServerInfo();
	ServerInfo(const ServerInfo& other);
	ServerInfo& operator=(const ServerInfo& other);
	~ServerInfo();
	//=============METHODS ========================================================
	//helpers
	std::string 							spaceTrimmer(std::string str);
	size_t 									calcBodySizeInBites(size_t body_size);
	//=============VALIDATORS =====================================================
	void									validServerTimeOut(std::string value);
	void									validListen(std::string value);
	void									validServerName(std::string value);
	void									validIndex(std::string value);
	void									validClientMaxBodySize(std::string& value);
	int										allSimpleInputsValid();
	void									validErrorPath(std::string value);
	void									validErrorType(std::string value);
	void									defaultErrorSetting();
	//=============SETTERS ========================================================
	void									setServerTimeOut(std::string line, int& attribute);
	void									setListen(std::string line);
	void									setServerName(std::string line);
	void									setIndex(std::string line);
	void									setClientMaxBodySize(std::string line);
	//=============HELPER METHODS =================================================
	void 									updatePaths(std::filesystem::path absolute_path);
	void									pushToErrors(std::string line);
	void									locationIndexes();
	void									pushLocationsLines();
	void									parsingLocations();
};

