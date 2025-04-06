#pragma once

#include "../ServerInfo/ServerInfo.hpp"

// enum print{YES = 1, NO = 0};
struct ServerInfo;
class Http
{
	public:
		std::vector <ServerInfo>	servers_;
		std::filesystem::path		executable_folder_http_;
		std::vector<std::string>	lines;
		std::vector<std::string>	lines_without_semicolons_;
		std::vector<size_t>			server_indexes_;
		int valid_config;
		//Default
		Http();
		Http(const Http& other);
		Http& operator=(const Http& other);
		~Http();
		//parsers
		void						configLines(std::filesystem::path config_path);
		void						configLinesWithoutSemicolons();
		void						serverLines();
		int							validFormatForOneServer(size_t start, size_t end);
		int							validServersFormat();
};
