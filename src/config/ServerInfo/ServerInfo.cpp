#include "ServerInfo.hpp"

ServerInfo::ServerInfo() : 
	server_timeout(-1), 
	listen(-1), 
	server_name(""), 
	index(""), 
	client_max_body_size(-1), 
	errors(),
	locations(),
	executable_folder_server(""),
	www_path(""),
	errors_path(""),
	uploads_dir(""),
	valid_inputs(YES),
	valid_errors(YES),
	valid_locations(YES)
{

};
ServerInfo::ServerInfo(std::filesystem::path absolute_path)
{
	executable_folder_server = absolute_path;
	// www_path = absolute_path / "src" / "www";
	www_path = absolute_path / "www";
	errors_path = www_path / "errors";
	uploads_dir = www_path / "uploads";
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
	executable_folder_server = other.executable_folder_server;
	www_path = other.www_path;
	errors_path = other.www_path;
	uploads_dir = other.uploads_dir;
	valid_inputs = other.valid_inputs;
	valid_errors = other.valid_errors;
	valid_locations = other.valid_locations;
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
		executable_folder_server = other.executable_folder_server;
		www_path = other.www_path;
		errors_path = other.www_path;
		uploads_dir = other.uploads_dir;
		valid_inputs = other.valid_inputs;
		valid_errors = other.valid_errors;
		valid_locations = other.valid_locations;
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
//============================Validators======================================
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
	std::string sub = value.substr(value.size() - 5, value.size());
	if (strIsAlphaOr(value, '.') == NO ||
			(
				strIsAlphaOr(value, '.') == YES && 
				value.find_last_of('.') != value.find_first_of('.')
			) ||
			(
				sub != ".html"
			)
		)
		{
			valid_inputs = NO;
			return ;
		}
		std::ifstream path(www_path / value);
		if (!path)
		{
			std::cerr << www_path / value << "didn't open\n";
			valid_inputs = NO;
		}
};
void						ServerInfo::validClientMaxBodySize(std::string value)
{
	std::string sub = value.substr(0, value.size() - 1);
	char last_char_value = value[value.size() - 1];
	if 	(
			(strIsNumber(value) && std::stol(value) <= 10000000) || 
			(strIsNumber(sub) &&  last_char_value == 'm' && stol(sub) <=10)
		)
			return ;
	valid_inputs = NO;
};
int						ServerInfo::allSimpleInputsValid()
{
	return (valid_inputs);
};
void						ServerInfo::validErrorPath(std::string value)
{
	std::filesystem::path checking_path;
	if (value[0] == '.')
		checking_path = executable_folder_server / value.substr(2);
	else
		checking_path = value;
	std::ifstream check(checking_path);
	if (!check)
		valid_errors = NO;
};

void						ServerInfo::validErrorType(std::string value)
{
	size_t the_last_backslash = value.find_last_of('/');
	std::string error_type = value.substr(the_last_backslash + 1, 3);
	if (strIsNumber(error_type) && 
	all_posible_errors.find(std::stol(error_type)) != all_posible_errors.end())
		return ;
	valid_errors = NO; 
};

int						ServerInfo::allErrorsValid()
{
	return (valid_errors);
};
// void						ServerInfo::validLocation(std::string value)
// {

// };
// void						ServerInfo::allLocationValid(std::string value)
// {

// };
// void						ServerInfo::allServerInputsValid(std::string value)
// {

// };

//let's have a plan for the servers that they will work simultaneously
// I need to create a folder with the servers name with error pages the required one // or the 404 if no error page is not given
// and an upload folder that I ll send the errors and the upoads
// all they will live under the www folder 
// example 
// gigi_one
//   uploads
//		<all the uplaods they will be here>
//   errors
//		404.html
//      405.html
//		409.html
//		505.html