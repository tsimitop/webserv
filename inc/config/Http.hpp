#pragma once

#include "ServerInfo.hpp"

// enum print{YES = 1, NO = 0};
struct ServerInfo;
struct Http
{
	std::vector <ServerInfo>	servers_;
	std::filesystem::path		executable_root_http_;
	//-------------Parsing Attributes Start--------------------
	std::vector<std::string>	lines;
	std::vector<std::string>	lines_without_semicolons_;
	std::vector<size_t>			server_indexes_;
	//-------------Parsing Attributes End----------------------
	int valid_config_;
	//Default
	Http();
	Http(const Http& other);
	Http& operator=(const Http& other);
	~Http();
	void						configLines(std::filesystem::path config_path);
	void						configLinesWithoutSemicolons();
	void						serverIndexes();
	int							validFormatForOneServer(size_t start, size_t end);
	void						validServersFormat();
	void						preparingAndValidatingConfig(int argc, char* argv[]);
	void						parsingServers();
};
