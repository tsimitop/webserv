#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define QUIT "\033[0m"

class HttpResponse
{
private:
	std::unordered_map<int, std::string>	_statusReason; // Status Code and Reason Phrase
	int										_statusCode;
	std::string								_reasonPhrase;
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

	// Setters
	void	setStatusCode(int sc);
	void	setReasonPhrase(std::string rp);
};