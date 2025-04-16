#include "Cgi.hpp"

Cgi::Cgi(int poll_fd, const HttpRequest& request) : poll_fd_(poll_fd), cgi_request_(request)
{}

Cgi::~Cgi(){}
