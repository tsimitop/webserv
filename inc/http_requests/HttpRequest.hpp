#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
// #include <stoi>
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
	std::vector<std::string>						_bodyVector; //for POST key1=value1&key2=value2&key3=value3 
	std::string										_bodyComplete;
	std::string										_httpRequest;
	std::string										_method;
	std::string										_url; //target resource path
	std::string										_version;
	int												_port;
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
	std::string										getMethod(void) const;
	std::string										getUrl(void) const;
	std::string										getVersion(void) const;
	int												getPort(void) const;

	// Setters
	void	setHttpRequest(std::string req);
	void	setMethod(std::string meth);
	void	setUrl(std::string url);
	void	setVersion(std::string ver);

	// Parse
	void	readRequest(std::string& requestLine);
	void	parseRequestLine(std::string& line);
	void	parseLine(std::string line);
	void	extractPortFromHost();
	bool	isValid();
	bool	validatePost();
	// bool	validateDelete();
	void	parseMethod(std::string& line);
	void	parseUrl(std::string& line);
	void	parseHttpVersion(std::string& line);
	void	fillBody(std::string& requestLine);

	// Debug
	void	printHeaders(void) const;
	void	printRequest(void) const;
	void	printBody(void) const;
};