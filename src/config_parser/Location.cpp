#include "../../inc/config/Location.hpp"
Location::Location() : 
location_lines_(), 
executable_root_location_(),
valid_location_(YES),
client_max_body_size_(0),
allowed_methods_(), 
location_html_(""), 
uploads_dir_(""), 
uploads_html_(""), 
python_path_(""), 
redir_(""),
redir_status_(0),
cgi_map_()
{
};
Location::Location(std::filesystem::path absolute_path)
{
	executable_root_location_ = absolute_path;
};
Location::Location(const Location& other)
{
	location_lines_ = other.location_lines_;
	executable_root_location_ = other.executable_root_location_;
	valid_location_ = other.valid_location_;
	uploads_dir_ = other.uploads_dir_;
	location_html_ = other.location_html_;
	uploads_html_ = other.uploads_html_;
	python_path_ = other.python_path_;
	client_max_body_size_ = other.client_max_body_size_;
	allowed_methods_ = other.allowed_methods_;
	redir_ = other.redir_;
	redir_status_ = other.redir_status_;
	redir_location_ = other.redir_location_;
	is_redir_ = other.is_redir_;
	name_ = other.name_;
	//-----map of cgis-----------------------
	cgi_map_ = other.cgi_map_;

};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		location_lines_ = other.location_lines_;
		executable_root_location_ = other.executable_root_location_;
		valid_location_ = other.valid_location_;
		uploads_dir_ = other.uploads_dir_;
		uploads_html_ = other.uploads_html_;
		python_path_ = other.python_path_;
		client_max_body_size_ = other.client_max_body_size_;
		allowed_methods_ = other.allowed_methods_;
		redir_ = other.redir_;
		redir_status_ = other.redir_status_;
		redir_location_ = other.redir_location_;
		is_redir_ = other.is_redir_;
		name_ = other.name_;
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
		valid_location_ = NO;
};

void						Location::validClientMaxBodySize(std::string& value)
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
	valid_location_ = NO;
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
				valid_location_ = NO;
				return;
			}
		}
		return ;
	}
	else
		valid_location_ = NO;
};
int						Location::validLocation()
{
	return (valid_location_);
};

int		Location::validErrorRoot (std::string value, std::string root)
{
	size_t the_first_backslash = value.find_first_of('/');
	size_t the_second_backslash = value.substr(value.find_first_of('/') + 1, value.size()).find_first_of('/'); // I m finding the ./ | <checking_root> | / errors / 404.html

	if (the_first_backslash == the_second_backslash)
		return NO;
	//------------DEBUGGING-------------------------
	std::string config_root = root;
	(void)config_root;
	//----------------------------------------------
	std::string checking_root = value.substr(the_first_backslash + 1, the_second_backslash);
	if (checking_root != root)
		return NO;
	return YES;
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
		if (valid_location_ != NO)
			client_max_body_size_ = std::stol(value);
		else
			printError("client_max_body_size", line);
	}
};

void	Location::setAllowedMethods(std::string line)
{
	validMethods(line);
	if (valid_location_ != NO)
	{
		std::stringstream l(line);
		std::string key, eq, value;
		l >> key >> eq;
		while (l >> value)
			allowed_methods_.insert(allowed_methods_.begin(), value);

	}
	else
		printError("allowed method", line);
};
void	Location::settingTheRightPath(std::string value, std::filesystem::path& p)
{
	if (value[0] == '.')
		p = executable_root_location_ / "src" / value.substr(2);
	else
		p = executable_root_location_ / "src" / value.substr(1);
}

void	Location::setPath (std::string line, std::filesystem::path& attribute, std::string root)
{
	validPath(line);
	std::stringstream l(line);
	std::string key, eq, value;
	l >> key >> eq >> value;
	if (!(valid_location_ = validErrorRoot(value, root)))
	{
		printError("path", line);
		return ;
	}
	settingTheRightPath(value, attribute);
};

void	Location::pushCgiMap(std::string line)
{
	validPath(line);
	std::stringstream l(line);
	std::string key, eq, value;
	l >> key >> eq >> value;
	std::filesystem::path p;
	if (valid_location_ != NO)
		settingTheRightPath(value, p);
	if (cgi_map_.find(p) == cgi_map_.end())
	{
		std::string cgi_key =	value[0] == '/' ? 
									value.substr(1, value.find_first_of('/', 1)) : 
									value.substr(2, value.find_first_of('/', 2));
		std::ifstream cgi_path(p);
		if (!cgi_path)
		{
			std::cerr << "Error CGI: File" << p << " is not opening\n";
			valid_location_ = NO;
			return ;
		}
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

void							printError(std::string type, std::string line)
{
	std::cerr << RED << "Error " << type <<": " << line << " is not valid!" << QUIT <<std::endl;
};

std::string							decodingHexToAscii(std::string filename)
{
	std::ostringstream d;
	for (size_t i = 0; i != filename.length(); i++)
	{
		if (filename[i] == '%' && i + 2 < filename.length()) //distinctively lower than the 2
		{
			std::string hex = filename.substr(i + 1, 2);
			int c = std::stoi(hex, nullptr, 16);
			d << (char)c;
			i += 2;
		}
		else
			d << filename[i];
	}
	(void)d.str();
	return d.str();
}

void	Location::setRedir (std::string line, std::string& attribute)
{
	(void)attribute;
	is_redir_ = true;
	std::stringstream l(line);
	std::string key, eq, status_code, new_location;
	l >> key >> eq >> status_code >> new_location;
	std::cout << "status code = " << status_code << "\n"
	<< "new_location = " << new_location << std::endl;
	redir_status_ = stoi(status_code);
	redir_location_ = new_location;
	std::cout << redir_status_ << " is redir status\n";
};