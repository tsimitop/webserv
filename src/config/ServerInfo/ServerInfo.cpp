#include "ServerInfo.hpp"

ServerInfo::ServerInfo() : 
	server_timeout(-1), 
	listen(-1), 
	server_name(""), 
	index(""), 
	client_max_body_size(-1), 
	errors(),
	locations(),
	executable_path(""),
	www_path(""),
	errors_path(""),
	uploads_path(""),
	valid_inputs(YES)
{

};
ServerInfo::ServerInfo(std::filesystem::path absolute_path)
{
	executable_path = absolute_path;
	www_path = absolute_path / "src" / "www";
	errors_path = www_path / "errors";
	uploads_path = www_path / "uploads";
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
	executable_path = other.executable_path;
	www_path = other.www_path;
	errors_path = other.www_path;
	uploads_path = other.uploads_path;
	valid_inputs = other.valid_inputs;
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
		executable_path = other.executable_path;
		www_path = other.www_path;
		errors_path = other.www_path;
		uploads_path = other.uploads_path;
		valid_inputs = other.valid_inputs;
	}
	return *this;
};
ServerInfo::~ServerInfo(){};
//methods
size_t ServerInfo::calcBodySizeInBites(size_t body_size)
{
	return (body_size * 1024);
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


int								strIsNumber(std::string str)
{
	for(char s : str)
		if (!isdigit(s))
			return NO;
	return YES;
};

int								strIsAlphaOr(std::string str, char extraChar)
{
	for(char s : str)
		if (!isalpha(s) && s != extraChar)
			return NO;
	return YES;
};
void						ServerInfo::validServerTimeOut(std::string value)
{
	// string has only digits
		if (strIsNumber(value) == NO || (strIsNumber(value) == YES && std::stol(value) > 100000))
			valid_inputs = NO;
};
void						ServerInfo::validListen(std::string value)
{
	if(strIsNumber(value) == NO || (strIsNumber(value) == YES && std::stol(value) > 9999))
		valid_inputs = NO;
};
void						ServerInfo::validServerName(std::string value)
{
	if (strIsAlphaOr(value, '_') == NO)
		valid_inputs = NO;
};
void						ServerInfo::validIndex(std::string value)
{
	if (strIsAlphaOr(value, '.') == NO ||
			(
				strIsAlphaOr(value, '.') == YES && 
				value.find_last_of('.') != value.find_first_of('.')
			) ||
			(
				value.substr(value.size() - 6, value.size()) != ".html"
			)
		)
		{
			valid_inputs = NO;
			return ;
		}
		std::ifstream path(www_path / value);
		if (!path)
			valid_inputs = NO;
};
void						ServerInfo::validClientMaxBodySize(std::string value)
{
	if (!strIsNumber(value) || (strIsNumber(value) && std::stol(value) > 10000000) || (strIsNumber(value.substr(0, value.size() - 1)) == YES && value[value.size() - 1] != 'm'))
		valid_inputs = NO;
};
void						ServerInfo::allSimpleInputsValid(std::string value)
{
	
};
void						ServerInfo::validErrorType(std::string value)
{

};
void						ServerInfo::validErrorPath(std::string value)
{

};

void						ServerInfo::allErrorsValid(std::string value)
{

};
void						ServerInfo::validLocation(std::string value)
{

};
void						ServerInfo::allLocationValid(std::string value)
{

};
void						ServerInfo::allServerInputsValid(std::string value)
{

};