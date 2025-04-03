#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "../Location/Location.hpp"

struct Server 
{
	//Default
	Server();
	Server(const Server& other);
	Server& operator=(const Server& other);
	~Server();

	int server_timeout;
	int listen;
	std::string server_name;
	std::string index;
	long long client_max_body_size;
	std::vector <int> errors;
	std::vector <Location> locations;
};