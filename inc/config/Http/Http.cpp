#include "Http.hpp"
//=======================Default Http===============================

Http::Http() : servers()
{
};
Http::Http(const Http& other)
{
	if (other.servers.empty() != 1)
		for (Server s : other.servers)
			servers.push_back(s);
};
Http& Http::operator=(const Http& other)
{
	if (this != &other)
	{
		if (other.servers.empty() != 1)
		for (Server s : other.servers)
			servers.push_back(s);
	}
	return *this;
};
Http::~Http()
{

};
size_t Http::calcBodySizeInBites(size_t body_size)
{
	return (body_size * 1024);
};
int		Http::hasCurlyBrackets(std::vector<std::string> lines)
{
	for (std::string l : lines)
	{

	}
	
};
int		Http::hasSemicolon(std::string str)
{
	return (str[str.size() - 1] == ';');
};
std::string Http::fastTrimmer(std::string str)
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
	std::cout << "str[i]: " << str[i] << ", str[j]: " << str[j] << "\n";
	res = str.substr(i, j-i + 1);
	return res;
};