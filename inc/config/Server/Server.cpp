#include "Server.hpp"

Server::Server() : 
	server_timeout(-1), 
	listen(-1), 
	server_name(""), 
	index(""), 
	client_max_body_size(-1), 
	errors(),
	locations()
{

};
Server::Server(const Server& other)
{
	server_timeout = other.server_timeout; 
	listen = other.listen ; 
	server_name = other.server_name; 
	index = other.index; 
	client_max_body_size = other.client_max_body_size;
if (other.errors.empty() != 1)
	for (int e : other.errors)
		errors.push_back(e);
if (other.locations.empty() != 1)
	for (Location l : other.locations)
		locations.push_back(l);
};
Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		server_timeout = other.server_timeout; 
		listen = other.listen ; 
		server_name = other.server_name; 
		index = other.index; 
		client_max_body_size = other.client_max_body_size;
	if (other.errors.empty() != 1)
		for (int e : other.errors)
			errors.push_back(e);
	if (other.locations.empty() != 1)
		for (Location l : other.locations)
			locations.push_back(l);
	}
	return *this;
};
Server::~Server()
{
};