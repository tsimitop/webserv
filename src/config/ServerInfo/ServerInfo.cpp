#include "ServerInfo.hpp"

ServerInfo::ServerInfo() : 
	server_timeout(-1), 
	listen(-1), 
	server_name(""), 
	index(""), 
	client_max_body_size(-1), 
	errors(),
	locations(),
	valid_inputs(YES);
{

};
ServerInfo::ServerInfo(const ServerInfo& other)
{
	server_timeout = other.server_timeout; 
	listen = other.listen ; 
	server_name = other.server_name; 
	index = other.index; 
	client_max_body_size = other.client_max_body_size;
if (other.errors.empty() != 1)
	for (int e : other.errors)
		errors.push_back(e);
if (other.locations.empty() != 1)
	for (Location l : other.locations)
		locations.push_back(l);
};
ServerInfo& ServerInfo::operator=(const ServerInfo& other)
{
	if (this != &other)
	{
		server_timeout = other.server_timeout; 
		listen = other.listen ; 
		server_name = other.server_name; 
		index = other.index; 
		client_max_body_size = other.client_max_body_size;
	if (other.errors.empty() != 1)
		for (int e : other.errors)
			errors.push_back(e);
	if (other.locations.empty() != 1)
		for (Location l : other.locations)
			locations.push_back(l);
	}
	return *this;
};
ServerInfo::~ServerInfo(){};
//methods
size_t ServerInfo::calcBodySizeInBites(size_t body_size)
{
	return (body_size * 1024);
};

int		ServerInfo::hasSemicolon(std::string str)
{
	return (str[str.size() - 1] == ';');
};

std::string 				ServerInfo::spaceTrimmer(std::string str)
{
	std::string res;
	size_t i;
	for (i = 0; i != str.size() ; i++)
		if (!isspace(str[i]))
			break;
	int j;
	for (j = (int)str.size() - 1; j != -1; j--)
		if (!isspace(str[j]))
			break;
	res = str.substr(i, j-i + 1);
	return res;
};
