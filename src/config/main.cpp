#include "./Http/Http.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
		return 1;
	Http c;
	c.preparingAndValidatingConfig(argv);
	c.parsingServers();
	return 0;
}