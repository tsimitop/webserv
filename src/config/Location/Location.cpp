#include "Location.hpp"

Location::Location() : allowed_methods_(-1), location_html_(""), uploads_dir_(""), uploads_html_(""), redir_("")
{
	cgi_map_["cgi_one"] = "";
	cgi_map_["cgi_two"] = "";
	cgi_map_["cgi_three"] = "";
};
Location::Location(std::filesystem::path absolute_path)
{
	executable_folder_location_ = absolute_path;
};
Location::Location(const Location& other)
{
	uploads_dir_ = other.uploads_dir_;
	location_html_ = other.location_html_;
	uploads_html_ = other.uploads_html_;
	allowed_methods_ = other.allowed_methods_;
	redir_ = other.redir_;
	//-----map of cgis-----------------------
	cgi_map_ = other.cgi_map_;

};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		uploads_dir_ = other.uploads_dir_;
		uploads_html_ = other.uploads_html_;
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
//===================Methods======================================================
void						Location::validPath(std::string value)
{
	std::filesystem::path checking_path;
	if (value[0] == '.')
		checking_path = executable_folder_location_ / value.substr(2);
	else
		checking_path = value;
	std::ifstream check(checking_path);
	if (!check)
		valid_inputs_ = NO;
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
				valid_inputs_ = NO;
				break;
			}
		}
	}
};
int						Location::validLocation()
{
	return (valid_inputs_);
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