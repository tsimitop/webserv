#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>
// #include <getline>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define QUIT "\033[0m"

class HttpRequest
{
private:
	std::unordered_map<std::string, std::string>	_headers;
	std::string										_httpRequest;
	std::string										_method;
	std::string										_url;
	std::string										_version;
public:
	// Orthodox Canonical Class Form
	HttpRequest();
	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);
	~HttpRequest();

	// Parameterized constructor
	HttpRequest(std::string& request);
	
	// Getters
	std::unordered_map<std::string, std::string>	getHeaders(void) const;
	std::string										getHttpRequest(void) const;
	std::string 									getMethod(void) const;
	std::string 									getUrl(void) const;
	std::string 									getVersion(void) const;

	// Setters
	void	setHttpRequest(std::string req);
	void	setMethod(std::string meth);
	void	setUrl(std::string url);
	void	setVersion(std::string ver);

	// Parse
	void	readRequest(std::string requestLine);
	void	parseRequestLine(std::string& line);
	void	parseLine(std::string line);
	bool	isValid();

	// Debug
	void	printHeaders(void) const;
	void	printRequest(void) const;
};