#include "../../inc/config/Http.hpp"
//===============DEFAULT CONSTRUCTORS ===========================================

Http::Http() : 
	servers_(),
	active_servers_(),
	executable_root_http_(""), 
	lines(), 
	lines_without_semicolons_(), 
	server_indexes_(), 
	valid_config_(NO)
{
};
Http::Http(const Http& other)
{
	if (other.servers_.empty() != 1)
		for (ServerInfo s : other.servers_)
			servers_.push_back(s);
	if (other.active_servers_.empty() != 1)
		for (ServerInfo s : other.active_servers_)
				active_servers_.push_back(s);
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
		if (other.active_servers_.empty() != 1)
				for (ServerInfo s : other.active_servers_)
						active_servers_.push_back(s);
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
};
//===============METHODS=========================================================
//-------------LINES & INDEXES-------------
void Http::configLines(std::filesystem::path config_path)
{
	std::vector<std::string> res;
	std::vector<std::string> clean_res;
	std::string temp;
	std::ifstream config(config_path);
	if (!config)
	{
		std::cerr << RED <<  "Error: File" << config_path << "is not opening\n" <<QUIT;
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
		std::cerr <<RED << "Error: File is empty\n" << QUIT;
		return ;
	}
	for (std::string l : clean_res)
		lines.push_back(l);
};
void	Http::configLinesWithoutSemicolons()
{
	std::vector<std::string> lines_w_semicolons;

	for (ServerInfo& s : servers_)
	{
		for (std::string& l : s.lines_of_server_)
			while (l.find(';') == l.size() - 1)
				l = l.substr(0, l.size() - 1);
	}
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
//-------------VALIDATORS------------------
int		Http::validFormatForOneServer(size_t server_index)
{
	size_t index = 0;
	int open_curly_from_location = NO;
	int location_found = -2;
	std::vector<std::string> temp_server_lines = servers_[server_index].lines_of_server_;
	for (std::string l : temp_server_lines)
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
		if ( index != temp_server_lines.size() - 1 && 
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
		if (index != temp_server_lines.size() - 1 && location_found > 0 && key == "}")
		{
			location_found = -2;
			open_curly_from_location = NO;
		}
		if (index == temp_server_lines.size() - 1 && 
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
	int i = 0;
	for (ServerInfo& s : servers_)
	{
		s.valid_server_ = validFormatForOneServer(i);
		i++;
	}
};
void						Http::validPostParsing()
{
	for (ServerInfo& s : servers_)
		if (s.listen_ == 0 || s.server_name_ == "")
			s.valid_server_ = NO;
	for (ServerInfo& s : servers_)
		if (s.valid_server_ == NO)
		{
			std::cout << "//==================================================================================================\n";
			std::cout << "\\-/_\\-/-\\-/_\\-/-\\-/_\\-                              ________|||<------------------------------" << std::endl;
			std::cout << "/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\    /--------\\\\\\<----------------------/--------\\" << std::endl;
			std::cout << "\\-/_\\-/-\\-/_\\-/-\\-/_\\-/-\\-/_\\-/-\\                 | ^  -  ^ |||<----------------------|---------|" << std::endl;
			std::cout << RED<< "Error: "<< QUIT << (s.listen_ == 0 ? "????" : std::to_string(s.listen_))<< RED <<" >----SERVER- -IS- -[[=NOT==]]VALID->"<< QUIT <<" (|"<< RED <<"<8>"<< QUIT <<" | "<< RED <<"<*>"<< QUIT <<"|)))<--------------------(-----------)" << std::endl;
			std::cout << "/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_/-\\_                  \\  <\\_/>  ///<----------------------\\---------/" << std::endl;
			std::cout << "\\-\\_\\-\\-\\-\\_\\-\\-\\-\\_\\-\\-\\-\\_\\-\\-\\-\\_\\-\\-\\-\\_\\-\\-   \\ -[-]- ///<------------------------\\-------/" << std::endl;
			std::cout << " /-\\_/-\\_/-\\_/-\\_/-\\_/                              \\__v__///<--------------------------\\-----/" << std::endl;
			std::cout << "//==================================================================================================\n";
		}
		else
		{
			std::cout << GREEN << "Success: "<< s.listen_<< " server is valid!" << QUIT <<std::endl;
		}
	for (size_t i = 0; i!= servers_.size(); i++)
	{
		if (servers_[i].valid_server_ == YES)
		{
			valid_config_ = YES;
			break;
		}
	}
};
//-------------PARSING---------------------
void Http::preparingAndValidatingConfig(int argc, char* argv[])
{
	executable_root_http_ = 
	std::filesystem::canonical
	(
		std::filesystem::absolute(argv[0])
	).parent_path();
	std::filesystem::path config_path;
	if (argc == 1)
		config_path = executable_root_http_ / "src/config/default.conf";
	else if (argc == 2)
		config_path = executable_root_http_ / "src/config" / argv[1];
	configLines(config_path);
	serverIndexes();
	// creating the servers pushing the lines
	for (size_t i = 0; i + 1!= server_indexes_.size(); i++)
	{
		std::vector<std::string> temp_lines;
		for(size_t j = server_indexes_[i]; j != server_indexes_[i + 1]; j++)
			temp_lines.push_back(lines[j]);
		ServerInfo s;
		s.lines_of_server_ = temp_lines;
		servers_.push_back(s);
	}
	validServersFormat();
	configLinesWithoutSemicolons();
}

void Http::parsingServers()
{
	for (ServerInfo& s : servers_)
	{
		s.updatePaths(executable_root_http_);
		for (std::string l : s.lines_of_server_)
		{
			std::stringstream line(l);
			std::string k;
			line >> k;
			if (s.before_locations_ == YES)
			{
				if(k == "listen")
					s.setListen(l);
				else if (k == "server_name")
					s.setServerName(l);
				else if (k == "server_timeout")
					s.setServerTimeOut(l, s.server_timeout_);
				else if (k == "keepalive_timeout")
					s.setServerTimeOut(l, s.keep_alive_timeout_);
				else if (k == "send_timeout")
					s.setServerTimeOut(l, s.send_timeout_);
				else if (k == "index")
					s.setIndex(l);
				else if (k == "client_max_body_size")
					s.setClientMaxBodySize(l);
				else if (k == "error_pages")
					s.pushToErrors(l);
				else if (k == "location/" || k == "location")
					break;
			}
		}
		s.locationIndexes();
		s.pushLocationsLines();
		s.parsingLocations();
		for (Location l : s.locations_)
		{
			if (l.valid_location_ == NO)
				s.valid_server_ = NO;
		}
	}
	for (size_t i = 0; i + 1!= servers_.size(); i++)
	{
		for (size_t j = i + 1; j != servers_.size(); j++)
			if (servers_[i].listen_ == servers_[j].listen_)
				servers_[i].valid_server_ = servers_[j].valid_server_ = NO;
	}
	for (ServerInfo& s : servers_)
		if(s.valid_server_ == YES)
			active_servers_.push_back(s);
};