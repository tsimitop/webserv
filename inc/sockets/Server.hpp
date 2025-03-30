/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daspring <daspring@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 20:21:29 by daspring          #+#    #+#             */
/*   Updated: 2025/03/30 19:32:51 by daspring         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//	TODO: 	- copy (and move?) operators
//			- better structuring of the constructor: create meaningful functions

#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>

class	Server {
public:
	Server();
	~Server();

	void	run();

private:
	struct addrinfo		hints_{};
	struct addrinfo*	server_info_;
	int					server_socket_{-1};
};
