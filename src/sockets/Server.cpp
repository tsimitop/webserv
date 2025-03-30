/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daspring <daspring@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 18:43:37 by daspring          #+#    #+#             */
/*   Updated: 2025/03/30 19:45:51 by daspring         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/sockets/sockets.hpp"
#include "../../inc/sockets/Server.hpp"

//	TODO:	reusing the server address does not quite work yet.


Server::Server() {
// start up server
	// memset(&hints_, 0, sizeof(hints_));			// redundant, b/c declaration of hints_ via hints_{} already instructed the compiler to det all member values to zero
	hints_.ai_family = PF_UNSPEC;					// AF_INET, AF_INET6, AF_UNSPEC
	hints_.ai_socktype = SOCK_STREAM;				// SOCK_STREAM, SOCK_DGRAM
	hints_.ai_flags = AI_PASSIVE;					// b/c we later bind it

	int	status = getaddrinfo(NULL, PORT, &hints_, &server_info_);
	if (status != 0)
	{
		std::cerr << "getaddrinfo failed: " << gai_strerror(status) << "\n";
	}


	server_socket_ = socket(server_info_->ai_family, server_info_->ai_socktype, server_info_->ai_protocol);		// ERROR HANDLING
	if (server_socket_ < 0)
	{
		std::cout << "socket failed\n";
	}

	status = bind(server_socket_, server_info_->ai_addr, server_info_->ai_addrlen);								// ERROR HANDLING - reusing status??
	if (status == -1)
	{
		std::cerr << "bind failed: " << strerror(errno) << "\n";
	}
	int	reuse = 1;																						// does not work yet ...
	if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)))
	{
		std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
	}

	if (listen(server_socket_, 5) == -1)
	{
		std::cout << "listen failed\n";
	}
}

Server::~Server() {
// close socket
// free add_struct
	close(server_socket_);
	freeaddrinfo(server_info_);

}

void	Server::run() {
// start the loop
// handle the new connection / client
// close connection

	struct sockaddr_storage	their_addr;
	socklen_t				their_addr_size	= sizeof (their_addr);
	while(true)
	{
		int						new_connection_fd		= accept(server_socket_, (struct sockaddr* )&their_addr, &their_addr_size);		// ERROR HANDLING
		if (new_connection_fd < 0)
		{
			std::cout << "accept failed\n";
		}

		char	msg[] = "Juhei! A new friend!\n";
		send(new_connection_fd, msg, sizeof(msg), 0);
		while(true)
		{
			char	buffer[BUFF_LEN] = {0};
			recv(new_connection_fd, buffer, BUFF_LEN, 0);
			std::cout << buffer;// << std::endl;
			if (buffer[0] == '\n' || buffer[0] == '\r')
			{
				std::cout << "end of connection\n";
				break ;
			}
		}
		close(new_connection_fd);
	}
}

