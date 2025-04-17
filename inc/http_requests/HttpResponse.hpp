#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include "HttpRequest.hpp"
#include <ctime>
#include <cstdlib>
#include <filesystem>
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define QUIT "\033[0m"

class HttpRequest;

class HttpResponse
{
private:
	std::unordered_map<int, std::string>	statusReason_; // Status Code and Reason Phrase
	int										statusCode_;
	std::string								reasonPhrase_;
	std::string								contentType_;
	int										contentLength_;
	std::string								body_;
public:
	// Orthodox Canonical Class Form
	HttpResponse();
	HttpResponse(const HttpResponse& other);
	HttpResponse& operator=(const HttpResponse& other);
	~HttpResponse();

	// Parameterized constructor
	HttpResponse(int sc);
	
	// Getters
	int			getStatusCode(void) const;
	std::string	getReasonPhrase(void) const;
	std::string	getContentType(void) const;
	std::string	getBody(void) const;
	int			getContentLength(void) const;

	// Setters
	void	setStatusCode(int sc);
	void	setReasonPhrase(int sc);
	void	setContentType(std::string ctype);
	void	setBody(const std::string& body);
	void	setContentLength(int len);
	void	createResponse(int status_code, std::filesystem::path file);
	void	createCgiResponse(int status_code, std::string content);

	const std::string respond(const HttpRequest& req);
};
