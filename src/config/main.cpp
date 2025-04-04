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
	std::filesystem::path executable_path = 
			std::filesystem::canonical
			(
				std::filesystem::absolute(argv[0])
			).parent_path();
	std::filesystem::path config_path = executable_path / argv[1];
	lines = c.configLines(config_path);
	server_lines = c.serverLines(lines);
	int valid_servers = c.validServersFormat(lines, server_lines);
	// int is_valid = s.validFormatForOneServer(lines, 0, 20); //from the first server until the next server - 1 || end 
	std::cout << "is_valid == " << (valid_servers == 1 ? "YES" : "NO") << std::endl;
	// std::cout << "words : " << s.countWords("            server_timeout     =       10000;   ") << std::endl;

	return 0;
}