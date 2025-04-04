#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "../Server/Server.hpp"

enum print{YES = 1, NO = 0};

class Http
{
	public:
		std::vector <Server> servers;
		//Default
		Http();
		Http(const Http& other);
		Http& operator=(const Http& other);
		~Http();

		size_t 		calcBodySizeInBites(size_t body_size);
		int			hasCurlyBrackets(std::vector<std::string>);
		int			hasSemicolon(std::string str);
		std::string fastTrimmer(std::string str);
		//getters -setters
};