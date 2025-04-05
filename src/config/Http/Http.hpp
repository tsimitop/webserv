#pragma once

#include "../ServerInfo/ServerInfo.hpp"

// enum print{YES = 1, NO = 0};
struct ServerInfo;
class Http
{
	public:
		std::vector <ServerInfo> servers;
		std::filesystem::path executable_folder_http;
		//Default
		Http();
		Http(const Http& other);
		Http& operator=(const Http& other);
		~Http();
		//parsers
		std::vector<std::string>	configLines(std::filesystem::path config_path);
		std::vector<size_t>			serverLines(std::vector<std::string> lines);
		int							validFormatForOneServer(std::vector<std::string> lines, size_t start, size_t end);
		int							validServersFormat(std::vector<std::string> lines, std::vector<size_t> server_indexes);
};
std::string 				spaceTrimmer(std::string str);
int							countWords(std::string line);