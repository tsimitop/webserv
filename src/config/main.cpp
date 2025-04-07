#include "./Http/Http.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;
	Http c;
	c.executable_folder_http_ = 
			std::filesystem::canonical
			(
				std::filesystem::absolute(argv[0])
			).parent_path();
	std::filesystem::path config_path = c.executable_folder_http_ / argv[1];// calling the copy constructor for the executable path and by calling the assiment constructor
	c.configLines(config_path);
	c.serverIndexes();
	c.valid_config = c.validServersFormat();
	c.configLinesWithoutSemicolons();
	c.parsingServers();
	return 0;
}