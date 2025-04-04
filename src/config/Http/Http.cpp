#include "Http.hpp"
//=======================Default Http===============================

Http::Http() : servers()
{
};
Http::Http(const Http& other)
{
	if (other.servers.empty() != 1)
		for (ServerInfo s : other.servers)
			servers.push_back(s);
};
Http& Http::operator=(const Http& other)
{
	if (this != &other)
	{
		if (other.servers.empty() != 1)
		for (ServerInfo s : other.servers)
			servers.push_back(s);
	}
	return *this;
};
Http::~Http()
{

};

//parsers
std::vector<std::string> Http::configLines(std::filesystem::path config_path)
{
	std::vector<std::string> res;
	std::vector<std::string> clean_res;
	std::string temp;
	std::ifstream config(config_path);
	if (!config)
	{
		std::cerr << "Error: File" << config_path << "is not opening\n";
		return res;
	}
	while (std::getline(config, temp))
	{
		if (spaceTrimmer(temp)[0] == '#' || spaceTrimmer(temp).size() == 0)
			continue;
		res.push_back(temp);
	}
	config.close();
	for(std::string l : res)
	{
		size_t index_of_hash = std::min(l.find("#", 0), l.find("//", 0));
		if (index_of_hash != std::string::npos)
			clean_res.push_back(spaceTrimmer(l.substr(0,index_of_hash)));
		else
			clean_res.push_back(l);
		
	}
	return clean_res;
};


std::vector<size_t>			Http::serverLines(std::vector<std::string> lines)
{
	std::vector<size_t> server_lines;
	size_t i = 0;
	for (std::string l : lines)
	{
		std::stringstream ss(l);
		std::string word;
		ss >> word;
		if (word == "server")
			server_lines.push_back(i);
		i++;
	}
	server_lines.push_back(i);
	return server_lines;
};
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

int		Http::validFormatForOneServer(std::vector<std::string> lines, size_t start, size_t end)
{
	size_t index = 0;
	int open_curly_from_location = NO;
	int location_found = -2;
	std::vector<std::string> current_lines;
	for (size_t i = start; i != end + 1; i++)
		current_lines.push_back(lines[i]);
	for (std::string l : current_lines)
	{
		std::string key, equals, value;
		std::stringstream ss(l);
		ss >> key;
		if (index == 0 && countWords(l) == 1 && key == "server")
		{
			index++;
			continue;
		}
		if ((index == 1 || location_found + 1 == (int)index))
		{
			if (countWords(l) == 1 && key == "{")
			{
				if(location_found + 1 == (int)index)
					open_curly_from_location = YES;
			}
		   else
			   return NO;
		}
		if (key == "location")
		{
			location_found = index;
			ss >> equals;
			if (spaceTrimmer(equals) != "/" && key[key.size() - 1] != '/')
				return NO;
		}
		if (index != current_lines.size() - 1 && 
			key != "server" && 
			key != "location" && 
			key != "{" && 
			key != "}")
		{
			if (countWords(l) == 3)
			{
				ss >> equals >> value;
				if (equals == "=" && value[value.size() - 1] != ';')
					return NO;
				index++;
				continue;
			}
			else if (location_found > 0)
			{
				index++;
				continue;
			}
			else
				return NO;

		}
		if (index != current_lines.size() - 1 && location_found > 0 && key == "}")
		{
			location_found = -2;
			open_curly_from_location = NO;
		}
		if (index == current_lines.size() - 1 && 
			key == "}" && 
			open_curly_from_location == NO 
			&& location_found == -2)
			return YES;
		index++;
	}
	return NO;
};

int	Http::validServersFormat(std::vector<std::string> lines, std::vector<size_t> server_indexes)
{
	for (size_t i = 0; i + 1!= server_indexes.size(); i++)
	{
		std::vector<std::string> current_lines;
		for (size_t j = server_indexes[i]; j != server_indexes[i + 1]; j++)
			current_lines.push_back(lines[j]);
		if (validFormatForOneServer(current_lines, 0, current_lines.size() - 1) == NO)
			return NO;
	}
	return YES;
};
