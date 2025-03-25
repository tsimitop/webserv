#include "../../inc/sockets/sockets.hpp"

void	start_server()
{
	struct addrinfo		hints;
	struct addrinfo*	server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;					// AF_INET, AF_INET6, AF_UNSPEC
	hints.ai_socktype = SOCK_STREAM;				// SOCK_STREAM, SOCK_DGRAM
	hints.ai_flags = AI_PASSIVE;					// b/c we later bind it

	int					status;
	status = getaddrinfo(NULL, PORT, &hints, &server_info);														// ERROR HANDLING !! (using fprintf, stderr, gai_strerror ??)
	if (status != 0)
	{
		std::cerr << "getaddrinfo failed: " << gai_strerror(status) << "\n";
	}


	int					server_filedes;
	server_filedes = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);		// ERROR HANDLING
	if (server_filedes < 0)
	{
		std::cout << "socket failed\n";
	}

	status = bind(server_filedes, server_info->ai_addr, server_info->ai_addrlen);								// ERROR HANDLING - reusing status??
	if (status == -1)
	{
		std::cerr << "bind failed: " << strerror(errno) << "\n";
	}
	int	reuse = 1;																						// does not work yet ...
	if (setsockopt(server_filedes, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)))
	{
		std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
	}

	if (listen(server_filedes, 5) == -1)
	{
		std::cout << "listen failed\n";
	}

	struct sockaddr_storage	their_addr;
	socklen_t				their_addr_size	= sizeof (their_addr);

	while(true)
	{
		int						new_connectin_fd		= accept(server_filedes, (struct sockaddr* )&their_addr, &their_addr_size);		// ERROR HANDLING
		if (new_connectin_fd < 0)
		{
			std::cout << "accept failed\n";
		}

		char	msg[] = "Juhei! A new friend!\n";
		send(new_connectin_fd, msg, sizeof(msg), 0);
		while(true)
		{
			char	buffer[BUFF_LEN] = {0};							// isn't it better to instead use a cpp-string?
			recv(new_connectin_fd, buffer, BUFF_LEN, 0);
			std::cout << buffer;// << std::endl;
			if (buffer[0] == '\n' || buffer[0] == '\r')
			{
				std::cout << "end of connection\n";
				break ;
			}
		}
		close(new_connectin_fd);
	}
	freeaddrinfo(server_info);
}
