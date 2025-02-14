/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sockets.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daspring <daspring@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 12:51:41 by daspring          #+#    #+#             */
/*   Updated: 2025/02/14 12:19:52 by daspring         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define PORT "8080"
#define BUFF_LEN 4096

void	start_server()
{
	struct addrinfo		hints;
	struct addrinfo*	server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

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
	int						new_filedes		= accept(server_filedes, (struct sockaddr* )&their_addr, &their_addr_size);		// ERROR HANDLING
	if (new_filedes < 0)
	{
		std::cout << "accept failed\n";
	}
	
	send(new_filedes, "Juhei!\n", 9, 0);
	char	buffer[BUFF_LEN];							// isn't it better to instead use a cpp-string?
	recv(new_filedes, buffer, BUFF_LEN, 0);
	std::cout << buffer << "\n";

	close(new_filedes);
	freeaddrinfo(server_info);
}
