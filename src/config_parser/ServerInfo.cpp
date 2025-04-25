#include "../../inc/config/ServerInfo.hpp"

ServerInfo::ServerInfo() : 
	lines_of_server_(),
	location_indexes_(),
	keep_alive_timeout_(-1),
	send_timeout_(-1),
	server_timeout_(0), 
	listen_(0),
	root_(""),
	server_name_(""), 
	index(""), 
	client_max_body_size_(0), 
	errors(),
	locations_(),
	executable_root_server_(""),
	www_path_(""),
	errors_path_(""),
	uploads_dir_(""),
	valid_server_(YES),
	before_locations_(YES)
{
};
ServerInfo::ServerInfo(const ServerInfo& other)
{
	keep_alive_timeout_= other.keep_alive_timeout_;
	send_timeout_ = other.send_timeout_;
	server_timeout_ = other.server_timeout_; 
	listen_ = other.listen_ ;
	root_ = other.root_;
	listen_ = other.listen_ ;
	root_ = other.root_;
	server_name_ = other.server_name_; 
	index = other.index; 
	client_max_body_size_ = other.client_max_body_size_;
	errors = other.errors;
	locations_ = other.locations_;
	locations_ = other.locations_;
	lines_of_server_ = other.lines_of_server_;
	location_indexes_ = other.location_indexes_;
	executable_root_server_ = other.executable_root_server_;
	www_path_ = other.www_path_;
	errors_path_ = other.errors_path_;
	uploads_dir_ = other.uploads_dir_;
	valid_server_ = other.valid_server_;
	before_locations_ = other.before_locations_;
};
ServerInfo& ServerInfo::operator=(const ServerInfo& other)
{
	if (this != &other)
	{
		keep_alive_timeout_= other.keep_alive_timeout_;
		send_timeout_ = other.send_timeout_;
		server_timeout_ = other.server_timeout_; 
		listen_ = other.listen_ ;
		root_ = other.root_; 
		listen_ = other.listen_ ;
		root_ = other.root_; 
		server_name_ = other.server_name_; 
		index = other.index; 
		client_max_body_size_ = other.client_max_body_size_;
		errors = other.errors;
		locations_ = other.locations_;
		locations_ = other.locations_;
		lines_of_server_ = other.lines_of_server_;
		location_indexes_ = other.location_indexes_;
		executable_root_server_ = other.executable_root_server_;
		www_path_ = other.www_path_;
		errors_path_ = other.errors_path_;
		uploads_dir_ = other.uploads_dir_;
		valid_server_ = other.valid_server_;
		before_locations_ = other.before_locations_;
	}
	return *this;
};
ServerInfo::~ServerInfo(){};
//methods
size_t ServerInfo::calcBodySizeInBites(size_t body_size)
{
	return (body_size);
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
	(void)value;
		if (strIsNumber(value) == NO || (strIsNumber(value) == YES && std::stol(value) > 100000))
			valid_server_ = NO;
};
void						ServerInfo::validListen(std::string value)
{
	if(strIsNumber(value) == NO || (strIsNumber(value) == YES && std::stol(value) > 9999))
		valid_server_ = NO;
};

void						ServerInfo::validServerName(std::string value)
{
	if (value != "127.0.0.1" && value != "localhost")
		valid_server_ = NO;
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
			valid_server_ = NO;
			return ;
		}
		std::ifstream path(www_path_ / value);
		if (!path)
		{
			std::cerr << RED << www_path_ / value << "didn't open\n" << QUIT;
			valid_server_ = NO;
		}
};
void						ServerInfo::validClientMaxBodySize(std::string& value)
{
	std::string sub = value.substr(0, value.size() - 1);
	char last_char_value = value[value.size() - 1];
	if 	((strIsNumber(value) && std::stol(value) <= 10000000 && std::stol(value) > 1024))
			return ;
	if 	((strIsNumber(sub) &&  last_char_value == 'm' && stol(sub) <=10))
	{
		value = sub + "000000";
		return ;
	}
	if 	((strIsNumber(sub) &&  last_char_value == 'k' && stol(sub) <=10000))
	{
		value = sub + "000";
		return ;
	}
	valid_server_ = NO;
};

int						ServerInfo::allSimpleInputsValid()
{
	return (valid_server_);
};
void						ServerInfo::validErrorPath(std::string value)
{
	std::filesystem::path checking_path;
	if (value[0] == '.')
		checking_path = executable_root_server_ / "src" /value.substr(2);
	else
		checking_path = value;
	std::ifstream check(checking_path);
	if (!check)
	{
		valid_server_ = NO;
		return ;
	}
};

void				ServerInfo::validErrorType(std::string value)
{
	size_t the_last_backslash = value.find_last_of('/');
	std::string error_type = value.substr(the_last_backslash + 1, 3);
	if (strIsNumber(error_type) && 
	all_posible_errors.find(std::stol(error_type)) != all_posible_errors.end())
		return ;
	valid_server_ = NO; 
};
void 					ServerInfo::defaultErrorSetting()
{
	errors[300] = errors_path_ / "300.html";
	errors[301] = errors_path_ / "301.html";
	errors[400] = errors_path_ / "400.html";
	errors[404] = errors_path_ / "404.html";
	errors[405] = errors_path_ / "405.html";
	errors[500] = errors_path_ / "500.html";
	errors[504] = errors_path_ / "504.html";
	errors[505] = errors_path_ / "505.html";
};

int		ServerInfo::validErrorRoot (std::string value)
{
	size_t the_first_backslash = value.find_first_of('/');
	size_t the_second_backslash = value.substr(value.find_first_of('/') + 1, value.size()).find_first_of('/'); // I m finding the ./ | <checking_root> | / errors / 404.html

	if (the_first_backslash == the_second_backslash)
		return NO;
	//------------DEBUGGING-------------------------
	std::string config_root = root_;
	(void)config_root;
	//----------------------------------------------
	std::string checking_root = value.substr(the_first_backslash + 1, the_second_backslash);
	if (checking_root != root_)
		return NO;
	return YES;
};

void						ServerInfo::setServerTimeOut(std::string line, int& attribute)
{
	std::stringstream current_line(line);
	if (countWords(line) == 3)
	{
		std::string key, eq, value;
		current_line >>key >> eq >> value;
		validServerTimeOut(value);
		if (valid_server_ != NO)
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
		if (valid_server_ != NO)
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
		if (valid_server_ != NO)
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
		if (valid_server_ != NO)
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
		if (valid_server_ != NO)
			client_max_body_size_ = std::stol(value);
	}
};
void 						ServerInfo::updatePaths(std::filesystem::path absolute_path)
{
	root_ = (root_ == "") ? "www" : root_;
	std::filesystem::path checking_path = absolute_path / "src" / root_;
	std::ifstream check(checking_path);
	if	(
				std::filesystem::is_directory(checking_path) &&
				(std::filesystem::exists(checking_path))
		)
	{
		executable_root_server_ = absolute_path;
		www_path_ = checking_path;
		errors_path_ = www_path_ / "errors";
		uploads_dir_ = www_path_ / "uploads";
		defaultErrorSetting();
	}
	else
		valid_server_ = 0;
};
void						ServerInfo::pushToErrors(std::string line)
{
	std::stringstream current_line(line);
	if (countWords(line) != 3)
	{
		valid_server_ = NO;
		return ;
	}
	std::string key, eq, value;
	current_line >>key >> eq >> value;
	validErrorPath(value);
	validErrorType(value);
	size_t the_last_backslash = value.find_last_of('/');
	std::string error_type = value.substr(the_last_backslash + 1, 3);
	if (!(valid_server_ = validErrorRoot(value)))
		return ;
	if (
			std::stoll(error_type) > 511 || std::stoll(error_type) < 100  || 
			all_posible_errors.find(std::stoi(error_type)) == all_posible_errors.end()
		)
	{
		valid_server_ = NO;
		return ;
	}
	if ( errors.find(std::stoi(error_type)) == errors.end())
		errors[std::stoi(error_type)] = value[0] == '.' ? 
										executable_root_server_ / value.substr(2) : 
										(std::filesystem::path)value;
};
void			ServerInfo::locationIndexes()
{
	int inside_location = NO;
	for (size_t index = 0; index != lines_of_server_.size(); index++)
	{
		std::stringstream	line(lines_of_server_[index]);
		// std::cout << "Location line = " << line.str() << std::endl;
		std::string k, name;
		line >> k;
		line >> name >> name;
		
		if (k == "location" || k == "location/")
		{
			// std::cout << "Location index = " << index << std::endl;
			// std::cout << "Location name = " << name << std::endl;
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
		Location location(executable_root_server_);
		location.valid_location_ = 1;
		for (size_t j = location_indexes_[i]; j != location_indexes_[i + 1] + 1; j++)
		{
			location.location_lines_.push_back(lines_of_server_[j]);
			// std::cout << "lines_of_server_[j] = ";
			// std::cout << lines_of_server_[j] << std::endl;
		}
		locations_.push_back(location);
	}
};

void	ServerInfo::parsingLocations()
{
	for (Location& location : locations_)
	{
		location.redir_status_ = 0;
		location.is_redir_ = false;
		for (std::string line : location.location_lines_)
		{
			std::stringstream l(line);
			std::string key, eq;
			l >> key >> eq >> eq;
			if (key == "location" || key == "location/")
			{
				// std::cout << eq << " = EQ\n";
				location.name_ = eq;
			}
			else if (key == "client_max_body_size")
				location.setClientMaxBodySize(line);
			else if (key == "allow_methods")
				location.setAllowedMethods(line);
			else if(key == "location_html")
				location.setPath(line, location.location_html_, root_);
			else if(key == "uploads_dir")
			{
				location.setPath(line, location.uploads_dir_, root_);
				if (location.valid_location_ == YES)
					uploads_dir_ = location.uploads_dir_;
			}
			else if(key == "upload_html")
				location.setPath(line, location.uploads_html_, root_);
			else if(key == "cgi_extension")
				location.pushCgiMap(line);
			else if(key == "redirect")
			{
				location.setRedir(line, location.redir_);
			}
			// else
			// 	printError("debug",line);
			if (location.valid_location_ == NO)
				break;
		}
	}
};
