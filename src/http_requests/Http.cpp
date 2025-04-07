#include "../../inc/config/Http.hpp"
//=======================Default Http===============================

Http::Http() : 
	servers_(), 
	executable_root_http_(""), 
	lines(), 
	lines_without_semicolons_(), 
	server_indexes_(), 
	valid_config_(YES)
{
};
Http::Http(const Http& other)
{
	if (other.servers_.empty() != 1)
		for (ServerInfo s : other.servers_)
			servers_.push_back(s);
	executable_root_http_ = other.executable_root_http_;
	lines = other.lines;
	lines_without_semicolons_ = other.lines_without_semicolons_;
	server_indexes_ = other.server_indexes_;
	valid_config_ = other.valid_config_;
};
Http& Http::operator=(const Http& other)
{
	if (this != &other)
	{
		if (other.servers_.empty() != 1)
		for (ServerInfo s : other.servers_)
			servers_.push_back(s);
		executable_root_http_ = other.executable_root_http_;
		lines = other.lines;
		lines_without_semicolons_ = other.lines_without_semicolons_;
		server_indexes_ = other.server_indexes_;
		valid_config_ = other.valid_config_;
	}
	return *this;
};
Http::~Http()
{
};;
//parsers
void Http::configLines(std::filesystem::path config_path)
{
	std::vector<std::string> res;
	std::vector<std::string> clean_res;
	std::string temp;
	std::ifstream config(config_path);
	if (!config)
	{
		std::cerr << "Error: File" << config_path << "is not opening\n";
		return ;
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
	if (clean_res.empty() || (!clean_res.empty() &&clean_res[0].empty()))
	{
		std::cerr << "Error: File is empty\n";
		return ;
	}
	for (std::string l : clean_res)
		lines.push_back(l);
};
void	Http::configLinesWithoutSemicolons()
{
	std::vector<std::string> lines_w_semicolons;
	for (std::string l : lines)
	{
		if (l.find(';') == l.size() - 1)
		{
			std::string l_without_semicolon = l.substr(0, l.size() - 1);
			lines_w_semicolons.push_back(l_without_semicolon);
		}
		else
			lines_w_semicolons.push_back(l);

	}
	for (std::string l : lines_w_semicolons)
		lines_without_semicolons_.push_back(l);
};

void	Http::serverIndexes()
{
	size_t i = 0;
	for (std::string l : lines)
	{
		std::stringstream ss(l);
		std::string word;
		ss >> word;
		if (word == "server")
			server_indexes_.push_back(i);
		i++;
	}
	server_indexes_.push_back(i);
};

int		Http::validFormatForOneServer(size_t start, size_t end)
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
		if ((key == "location" || key == "location/"))
		{
			if (countWords(l) > 3)
				return NO;
			location_found = index;
			ss >> equals;
			if (key[key.size() - 1] != '/' && spaceTrimmer(equals) != "" && spaceTrimmer(equals) != "/" )
				return NO;
		}
		if ( index != current_lines.size() - 1 && 
			key != "server" && 
			key != "location" && 
			key != "{" && 
			key != "}")
		{
			if (countWords(l) == 3)
			{
				ss >> equals >> value;
				if (equals == "=" && 
						(
							value[value.size() - 1] != ';' || 
							(	
								value.find(';')!= std::string::npos && 
								value.find_first_of(';') !=value.find_last_of(';')
							)
						)
					)
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

void	Http::validServersFormat()
{
	for (size_t i = 0; i + 1!= server_indexes_.size(); i++) // 0 , 28, 42, 57 | , 69
	{
		std::vector<std::string> current_lines;
		for (size_t j = server_indexes_[i]; j != server_indexes_[i + 1]; j++)
			current_lines.push_back(lines[j]);
		if (validFormatForOneServer(server_indexes_[i], server_indexes_[i + 1] - 1) == NO)
		{
			valid_config_ = NO;
			return ;
		}
	}
	valid_config_ = YES;
};

void Http::parsingServers()
{
	for (size_t i = 0; i != server_indexes_.size() - 1; i++)
	{
		std::vector<std::string> current_non_semi;
		for (size_t j = server_indexes_[i]; j != server_indexes_[i + 1]; j++)
			current_non_semi.push_back(lines_without_semicolons_[j]);
		ServerInfo s(executable_root_http_);
		s.valid_inputs_ = YES;
		s.lines_of_server_ = current_non_semi;
		for (std::string l : s.lines_of_server_)
		{
			std::stringstream line(l);
			std::string k;
			line >> k;
			if (k == "keepalive_timeout")
				s.setServerTimeOut(l, s.keep_alive_timeout_);
			else if (k == "send_timeout")
				s.setServerTimeOut(l, s.send_timeout_);
			else if (k == "server_timeout")
				s.setServerTimeOut(l, s.server_timeout_);
			else if(k == "listen")
				s.setListen(l);
			else if (k == "server_name")
				s.setServerName(l);
			else if (k == "index")
				s.setIndex(l);
			else if (k == "client_max_body_size")
				s.setClientMaxBodySize(l);
			else if (k == "error_pages")
				s.pushToErrors(l);
			else
				;
		}
		s.locationIndexes();
		s.pushLocationsLines();//creating the locations and pushing the corresponding location lines
		s.parsingLocations();
		servers_.push_back(s);
	}
};

void Http::preparingAndValidatingConfig(int argc, char* argv[])
{
	executable_root_http_ = 
	std::filesystem::canonical
	(
		std::filesystem::absolute(argv[0])
	).parent_path().parent_path().parent_path();
	std::filesystem::path config_path;
	// std::cout << executable_root_http_ <<std::endl;
	if (argc == 1)
		config_path = executable_root_http_ / "src/config/default.conf";
	else if (argc == 2)
		config_path = executable_root_http_ / argv[1];// calling the copy constructor for the executable path and by calling the assiment constructor
	configLines(config_path);
	if (valid_config_ == YES)
	{
		serverIndexes();
		validServersFormat();
		if (valid_config_ == YES)
			configLinesWithoutSemicolons();
	}
}
