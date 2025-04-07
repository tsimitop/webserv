#include "Location.hpp"

Location::Location() : 
location_lines_(), 
executable_folder_location_(),
valid_inputs_(YES),
client_max_body_size_(),
allowed_methods_(), 
location_html_(""), 
uploads_dir_(""), 
uploads_html_(""), 
redir_(""),
cgi_map_()
{
};
Location::Location(std::filesystem::path absolute_path)
{
	executable_folder_location_ = absolute_path;
};
Location::Location(const Location& other)
{
	location_lines_ = other.location_lines_;
	executable_folder_location_ = other.executable_folder_location_;
	valid_inputs_ = other.valid_inputs_;
	uploads_dir_ = other.uploads_dir_;
	location_html_ = other.location_html_;
	uploads_html_ = other.uploads_html_;
	client_max_body_size_ = other.client_max_body_size_;
	allowed_methods_ = other.allowed_methods_;
	redir_ = other.redir_;
	//-----map of cgis-----------------------
	cgi_map_ = other.cgi_map_;

};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		location_lines_ = other.location_lines_;
		executable_folder_location_ = other.executable_folder_location_;
		valid_inputs_ = other.valid_inputs_;
		uploads_dir_ = other.uploads_dir_;
		uploads_html_ = other.uploads_html_;
		client_max_body_size_ = other.client_max_body_size_;
		allowed_methods_ = other.allowed_methods_;
		redir_ = other.redir_;
		//-----map of cgis-----------------------
		cgi_map_ = other.cgi_map_;
	}
	return *this;
};
Location::~Location()
{
};
//===================Vaidating Methods==============================================
void						Location::validPath(std::string line)
{
	std::stringstream l(line);
	std::string key, eq, value;
	l >> key >> eq >> value;
	std::filesystem::path checking_path;
	settingTheRightPath(value, checking_path);
	std::ifstream check(checking_path);
	if	(
			!check || 
			(
				std::filesystem::is_directory(checking_path) &&
				!(std::filesystem::exists(checking_path))
			)
		)
		valid_inputs_ = NO;
};

void						Location::validClientMaxBodySize(std::string& value)
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

void					Location::validMethods(std::string line)
{
	std::stringstream check_methods(line);
	std::string key, eq;
	check_methods >> key >> eq;
	if (countWords(line) - 2 <= 3)
	{
		std::string temp;
		while (check_methods >> temp)
		{
			if(temp != "POST" && temp != "GET" && temp != "DELETE")
			{
				valid_inputs_ = NO;
				return;
			}
		}
		return ;
	}
	else
		valid_inputs_ = NO;
};
int						Location::validLocation()
{
	return (valid_inputs_);
};
//===================Setting Methods================================================
void		Location::setClientMaxBodySize(std::string line)
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

void	Location::setAllowedMethods(std::string line)
{
	validMethods(line);
	if (valid_inputs_ != NO)
	{
		std::stringstream l(line);
		std::string key, eq, value;
		l >> key >> eq;
		while (l >> value)
			allowed_methods_.push_back(value);

	}
};
void	Location::settingTheRightPath(std::string value, std::filesystem::path& p)
{
	if (value[0] == '.')
		p = executable_folder_location_ / value.substr(2);
	else
		p = value;
}

void	Location::setPath (std::string line, std::filesystem::path& attribute)
{
	validPath(line);
	std::stringstream l(line);
	std::string key, eq, value;
	l >> key >> eq >> value;
	if (valid_inputs_ != NO)
		settingTheRightPath(value, attribute);
};
void	Location::pushCgiMap(std::string line)
{
	validPath(line);
	std::stringstream l(line);
	std::string key, eq, value;
	l >> key >> eq >> value;
	std::filesystem::path p;
	if (valid_inputs_ != NO)
		settingTheRightPath(value, p);
	if (cgi_map_.find(p) == cgi_map_.end())
	{
		std::string cgi_key =	value[0] == '/' ? 
									value.substr(1, value.find_first_of('/', 1)) : 
									value.substr(2, value.find_first_of('/', 2));
		cgi_map_[cgi_key] = p;
	}

};


//=========================Helper For Validators====================================
std::string 	spaceTrimmer(std::string str)
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

int countWords(std::string line)
{
	std::stringstream ss(line);
	std::string str;
	int words = 0;
	while (ss >> str)
		words++;
	return words;
}

int		strIsNumber(std::string str)
{
	for(char s : str)
		if (!isdigit(s))
			return NO;
	return YES;
};

int		strIsAlphaOr(std::string str, char extraChar)
{
	for(char s : str)
		if (!isalpha(s) && s != extraChar)
			return NO;
	return YES;
};