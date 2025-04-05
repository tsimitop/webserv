#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>

enum methods{GET = 1, POST = 2, DELETE = 4};

struct Location
{
	//Default
	Location();
	Location(const Location& other);
	Location& operator=(const Location& other);
	~Location();

	int	allowed_methods; // 1 is GET only, 3 is GET POST, 7 is GET POST DELETE
	std::string location_html;
	std::string uploads_dir;
	std::string uploads_html;
	std::string redir;
	std::map <std::string, std::string> cgi_map;
};