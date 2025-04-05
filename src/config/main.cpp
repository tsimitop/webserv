#include "./Http/Http.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;
	std::cout << "agrv[1]: " << argv[1] << std::endl;
	Http c;
	ServerInfo s;
	std::vector<size_t> server_lines;
	std::vector<std::string> lines;
	c.executable_folder_http = 
			std::filesystem::canonical
			(
				std::filesystem::absolute(argv[0])
			).parent_path();
	std::filesystem::path config_path = c.executable_folder_http / argv[1];
	s = ServerInfo(c.executable_folder_http);// calling the copy constructor for the executable path and by calling the assiment constructor

	c.servers.push_back(s);
	c.servers[0].validErrorType("./www/errors/503.html");
	lines = c.configLines(config_path);
	server_lines = c.serverLines(lines);
	int valid_servers = c.validServersFormat(lines, server_lines);
	std::cout << "is_valid == " << (valid_servers == 1 ? "YES" : "NO") << std::endl;

	return 0;
}