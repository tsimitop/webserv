#include "ServerInfo.hpp"

ServerInfo::ServerInfo() : 
	lines_of_server_(),
	location_indexes_(),
	keep_alive_timeout_(-1),
	send_timeout_(-1),
	server_timeout_(-1), 
	listen_(-1), 
	server_name_(""), 
	index(""), 
	client_max_body_size_(-1), 
	errors(),
	locations_(),
	executable_folder_server_(""),
	www_path_(""),
	errors_path_(""),
	uploads_dir_(""),
	valid_inputs_(YES),
	valid_errors_(YES),
	valid_locations_(YES)
{

};
ServerInfo::ServerInfo(std::filesystem::path absolute_path)
{
	executable_folder_server_ = absolute_path;
	// www_path_ = absolute_path / "src" / "www";
	www_path_ = absolute_path / "www";
	errors_path_ = www_path_ / "errors";
	uploads_dir_ = www_path_ / "uploads";
};
ServerInfo::ServerInfo(const ServerInfo& other)
{
	keep_alive_timeout_= other.keep_alive_timeout_;
	send_timeout_ = other.send_timeout_;
	server_timeout_ = other.server_timeout_; 
	listen_ = other.listen_ ; 
	server_name_ = other.server_name_; 
	index = other.index; 
	client_max_body_size_ = other.client_max_body_size_;
	errors = other.errors;
	if (other.locations_.empty() != 1)
		for (Location l : other.locations_)
			locations_.push_back(l);
	lines_of_server_ = other.lines_of_server_;
	location_indexes_ = other.location_indexes_;
	executable_folder_server_ = other.executable_folder_server_;
	www_path_ = other.www_path_;
	errors_path_ = other.www_path_;
	uploads_dir_ = other.uploads_dir_;
	valid_inputs_ = other.valid_inputs_;
	valid_errors_ = other.valid_errors_;
	valid_locations_ = other.valid_locations_;
};
ServerInfo& ServerInfo::operator=(const ServerInfo& other)
{
	if (this != &other)
	{
		keep_alive_timeout_= other.keep_alive_timeout_;
		send_timeout_ = other.send_timeout_;
		server_timeout_ = other.server_timeout_; 
		listen_ = other.listen_ ; 
		server_name_ = other.server_name_; 
		index = other.index; 
		client_max_body_size_ = other.client_max_body_size_;
		errors = other.errors;
		if (other.locations_.empty() != 1)
			for (Location l : other.locations_)
				locations_.push_back(l);
		lines_of_server_ = other.lines_of_server_;
		location_indexes_ = other.location_indexes_;
		executable_folder_server_ = other.executable_folder_server_;
		www_path_ = other.www_path_;
		errors_path_ = other.www_path_;
		uploads_dir_ = other.uploads_dir_;
		valid_inputs_ = other.valid_inputs_;
		valid_errors_ = other.valid_errors_;
		valid_locations_ = other.valid_locations_;
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
			valid_inputs_ = NO;
};
void						ServerInfo::validListen(std::string value)
{
	if(strIsNumber(value) == NO || (strIsNumber(value) == YES && std::stol(value) > 9999))
		valid_inputs_ = NO;
};
void						ServerInfo::validServerName(std::string value)
{
	if (strIsAlphaOr(value, '_') == NO)
		valid_inputs_ = NO;
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
			valid_inputs_ = NO;
			return ;
		}
		std::ifstream path(www_path_ / value);
		if (!path)
		{
			std::cerr << www_path_ / value << "didn't open\n";
			valid_inputs_ = NO;
		}
};
void						ServerInfo::validClientMaxBodySize(std::string& value)
{
	std::string sub = value.substr(0, value.size() - 1);
	char last_char_value = value[value.size() - 1];
	if 	((strIsNumber(value) && std::stol(value) <= 10000000))
			return ;
	if 	((strIsNumber(sub) &&  last_char_value == 'm' && stol(sub) <=10))
	{
		value = sub + "000000";
		return ;
	}
	valid_inputs_ = NO;
};
int						ServerInfo::allSimpleInputsValid()
{
	return (valid_inputs_);
};
void						ServerInfo::validErrorPath(std::string value)
{
	std::filesystem::path checking_path;
	if (value[0] == '.')
		checking_path = executable_folder_server_ / value.substr(2);
	else
		checking_path = value;
	std::ifstream check(checking_path);
	if (!check)
	{
		valid_errors_ = NO;
		return ;
	}
};

void						ServerInfo::validErrorType(std::string value)
{
	size_t the_last_backslash = value.find_last_of('/');
	std::string error_type = value.substr(the_last_backslash + 1, 3);
	if (strIsNumber(error_type) && 
	all_posible_errors.find(std::stol(error_type)) != all_posible_errors.end())
		return ;
	valid_errors_ = NO; 
};

int						ServerInfo::allErrorsValid()
{
	return (valid_errors_);
};

void						ServerInfo::setServerTimeOut(std::string line, int& attribute)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validServerTimeOut(value);
		if (valid_inputs_ != NO)
			attribute = std::stoi(value);
	}
};
void						ServerInfo::setListen(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validListen(value);
		if (valid_inputs_ != NO)
			listen_ = std::stoi(value);
	}
};
void						ServerInfo::setServerName(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validServerName(value);
		if (valid_inputs_ != NO)
			server_name_ = value;
	}
};
void						ServerInfo::setIndex(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validIndex(value);
		if (valid_inputs_ != NO)
			index = value;
	}
};
void						ServerInfo::setClientMaxBodySize(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validClientMaxBodySize(value);
		if (valid_inputs_ != NO)
			client_max_body_size_ = std::stol(value) * 1024;
	}
};
void						ServerInfo::pushToErrors(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validErrorPath(value);
		validErrorType(value);
		size_t the_last_backslash = value.find_last_of('/');
		std::string error_type = value.substr(the_last_backslash + 1, 3);
		if (valid_inputs_ != NO)
		{
			if (errors.find(std::stoi(error_type)) == errors.end()) // checking for duplicates
						errors[std::stoi(error_type)] = value[0] == '.' ? 
														executable_folder_server_ / value.substr(2) : 
														(std::filesystem::path)value;
			else
				valid_inputs_ = NO;
		}
			
	}
};
void			ServerInfo::locationIndexes()
{
	int inside_location = NO;
	for (size_t index = 0; index != lines_of_server_.size(); index++)
	{
		std::stringstream	line(lines_of_server_[index]);
		std::string k;
		line >> k;
		if (k == "location" || k == "location/")
		{
			location_indexes_.push_back(index);
			inside_location = YES;
		}
		if (k == "}" && inside_location == YES)
		{
			location_indexes_.push_back(index);
			inside_location = NO;
		}
	}
};

void						ServerInfo::pushLocationsLines()
{
	for (size_t i = 0; i + 1 < location_indexes_.size(); i+=2)
	{
		Location location(executable_folder_server_);
		location.valid_inputs_ = 1;
		for (size_t j = location_indexes_[i]; j != location_indexes_[i + 1] + 1; j++)
			location.location_lines_.push_back(lines_of_server_[j]);
		locations_.push_back(location);
	}
};

void	ServerInfo::parsingLocations()
{
	for (Location location : locations_)
	{
		for (std::string line : location.location_lines_)
		{
			std::stringstream l(line);
			std::string key, eq, value;
			l >> key >> eq;
			if (key == "client_max_body_size")
				location.setClientMaxBodySize(line);
			else if (key == "allow_methods")
				location.setAllowedMethods(line);
			else if(key == "location_html")
				location.setPath(line, location.location_html_);
			else if(key == "uploads_dir")
				location.setPath(line, location.uploads_dir_);
			else if(key == "uploads_html")
				location.setPath(line, location.uploads_html_);
			else if(key == "redir")
				location.setPath(line, location.redir_);
			else if(key == "cgi_extension")
				location.pushCgiMap(line);
			else
				;
		}
	}
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