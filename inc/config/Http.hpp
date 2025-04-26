#pragma once

#include "ServerInfo.hpp"

// enum print{YES = 1, NO = 0};
struct ServerInfo;
struct Http
{
	std::map<std::string, int> accepted_keys;
	std::vector <ServerInfo>	servers_;
	std::vector <ServerInfo>	config_valid_servers_;
	std::vector<ServerInfo> 	active_servers_;
	std::filesystem::path		executable_root_http_;
	//===============PARSING ATTRIBUTES START =======================================
	std::vector<std::string>	lines;
	std::vector<std::string>	lines_without_semicolons_;
	std::vector<size_t>			server_indexes_;
	//-------------FLAG(S)---------------------
	int valid_config_;
	//===============DEFAULT CONSTRUCTORS ===========================================
	Http();
	Http(const Http& other);
	Http& operator=(const Http& other);
	~Http();
	//===============METHODS ========================================================
	//-------------LINES & INDEXES-------------
	void						configLines(std::filesystem::path config_path);
	int							acceptedAttributes();
	void						configLinesWithoutSemicolons();
	void						serverIndexes();
	void						pushLinesToServers();
	//-------------VALIDATORS------------------
	int							curliesAndSemicollonsForOneServer(size_t server_index);
	void						validServersFormat();
	void						validPostParsing();
	//-------------PARSING---------------------
	void						preparingAndValidatingConfig(int argc, char* argv[]);
	void						parsingServers();
	//-------------UPDATED METHODS-------------
};
