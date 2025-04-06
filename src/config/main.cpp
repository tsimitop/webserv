#include "./Http/Http.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;
	std::cout << "agrv[1]: " << argv[1] << std::endl;
	Http c;
	ServerInfo s;
	// std::vector<size_t> server_indexes_;
	// std::vector<std::string> lines;
	// std::vector<std::string> lines_without_semicolons_;
	c.executable_folder_http_ = 
			std::filesystem::canonical
			(
				std::filesystem::absolute(argv[0])
			).parent_path();
	std::filesystem::path config_path = c.executable_folder_http_ / argv[1];
	s = ServerInfo(c.executable_folder_http_);// calling the copy constructor for the executable path and by calling the assiment constructor

	c.servers_.push_back(s);
	c.servers_[0].setServerTimeOut("");
	c.configLines(config_path);
	c.serverLines();
	c.servers_[0].valid_inputs_ = c.validServersFormat();
	c.configLinesWithoutSemicolons();
	c.servers_[0].setServerTimeOut(c.lines_without_semicolons_[2]);
	std::cout << "server_timeout_: " << c.servers_[0].server_timeout_ << std::endl;
	std::cout << "is_valid == " << (c.servers_[0].valid_inputs_ == 1 ? "YES" : "NO") << std::endl;

	return 0;
}