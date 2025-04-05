#include "Location.hpp"

Location::Location() : allowed_methods(-1), location_html(""), uploads_dir(""), uploads_html(""), redir("")
{
	cgi_map["cgi_one"] = "";
	cgi_map["cgi_two"] = "";
	cgi_map["cgi_three"] = "";
};
Location::Location(std::filesystem::path absolute_path)
{
	executable_folder_location = absolute_path;
};
Location::Location(const Location& other)
{
	uploads_dir = other.uploads_dir;
	location_html = other.location_html;
	uploads_html = other.uploads_html;
	allowed_methods = other.allowed_methods;
	redir = other.redir;
	//-----map of cgis-----------------------
	cgi_map = other.cgi_map;

};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		uploads_dir = other.uploads_dir;
		uploads_html = other.uploads_html;
		allowed_methods = other.allowed_methods;
		redir = other.redir;
		//-----map of cgis-----------------------
		cgi_map = other.cgi_map;
	}
	return *this;
};
Location::~Location()
{
};
//===================Methods======================================================
void						Location::validPath(std::string value)
{
	std::filesystem::path checking_path;
	if (value[0] == '.')
		checking_path = executable_folder_location / value.substr(2);
	else
		checking_path = value;
	std::ifstream check(checking_path);
	if (!check)
		valid_inputs = NO;
};

void					Location::validMethods(std::string value)
{
	std::stringstream check_methods(value);
	if (countWords(value) <= 3)
	{
		std::string temp;
		while (check_methods >> temp)
		{
			if(temp != "POST" && temp != "GET" && temp != "DELETE")
			{
				valid_inputs = NO;
				break;
			}
		}
	}
};
int						Location::validLocation()
{
	return (valid_inputs);
};
//=========================Helper For Validators==============================
std::string 				spaceTrimmer(std::string str)
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