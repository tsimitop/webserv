#include "../../inc/sockets/sockets.hpp"
#include "../../inc/sockets/Server.hpp"
#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/http_requests/HttpResponse.hpp"

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


	server_socket_ = socket(server_info_->ai_family, server_info_->ai_socktype, server_info_->ai_protocol);
	if (server_socket_ < 0)
	{
		std::cout << "socket failed\n";
	}

	int	reuse = 1;
	if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)))
	{
		std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
	}
	status = bind(server_socket_, server_info_->ai_addr, server_info_->ai_addrlen);
	if (status == -1)
	{
		std::cerr << "bind failed: " << strerror(errno) << "\n";
	}

	if (listen(server_socket_, 5) == -1)
	{
		std::cout << "listen failed\n";
	}
}

Server::~Server() {
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
		int						client_socket		= accept(server_socket_, (struct sockaddr* )&their_addr, &their_addr_size);
		if (client_socket < 0)
		{
			std::cout << "accept failed\n";
		}
		handleClient(client_socket);
		close(client_socket);
	}
}


//	example function, could also be 'imported' from a different class (right?)
//	this function only takes the input from the client and echoes it.
void	Server::handleClient(int client_socket) {
	int server_error = 0;
	while(true)
	{
		char	buffer[BUFF_LEN] = {0};
		HttpRequest		request;
		HttpResponse	response;

		recv(client_socket, buffer, BUFF_LEN, 0);
		// std::cout << CYAN << "Request: " << buffer << QUIT << std::endl;
		std::string req = buffer;
		try
		{
			request.readRequest(req);
			if (!request.isValid())
				throw HttpRequest::httpParserException();
		}
		catch(...)
		{
			server_error = 1;
			std::cerr << RED << "Exception thrown: error while parsing http request.\n";
		}
		if (server_error == 0)
		{
			request.extractPortFromHost();
			response = request.performMethod();
			std::string res = response.respond(request);
			// std::cout << YELLOW << "'" << res << "'" << QUIT << std::endl;
			send(client_socket, res.c_str(), res.size(), 0);
		}
		break ;
	}
}
