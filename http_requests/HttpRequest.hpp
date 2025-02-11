#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <fstream>
// #include <getline>

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

	void	readRequest(std::ifstream&	requestFile);
	void	parseRequestLine(std::string& line);
	void	parseLine(std::string line);

	// Debug
	void	printHeaders(void) const;
};