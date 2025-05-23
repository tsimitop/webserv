#pragma once
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "HttpResponse.hpp"
#include "../config/Http.hpp"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define QUIT "\033[0m"

class HttpResponse;

class HttpRequest
{
private:
	std::map<int, std::filesystem::path>			available_errors_;
	std::filesystem::path							current_www_path_;
	std::unordered_map<std::string, std::string>	headers_;
	std::vector<std::string>						bodyVector_; //for POST key1=value1&key2=value2&key3=value3 
	std::string										bodyComplete_;
	std::string										httpRequest_;
	std::string										method_;
	std::string										url_; //target resource path
	std::string										version_;
	int												port_;
	std::string										content_length_;
	std::string										basePath_;
	std::string										filename_;
	ServerInfo										current_server_;
	bool											executed_;
	bool											is_redir_;
	bool											req_is_invalid_;
	bool											forbidden_meth_;
	std::filesystem::path							python_path_;
public:
	// Orthodox Canonical Class Form
	HttpRequest();
	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);
	~HttpRequest();

	// Parameterized constructor
	HttpRequest(const std::string& request, const ServerInfo& server_info);
	
	// Getters
	bool											isForbidden() const;
	bool											isRedirection() const;
	bool											isInvalid() const;
	std::unordered_map<std::string, std::string>	getHeaders(void) const;
	std::string										getHttpRequest(void) const;
	std::string										getMethod(void) const;
	std::string										getUrl(void) const;
	std::string										getVersion(void) const;
	std::string										getBasePath(void) const;
	std::string										getFilename(void) const;
	std::string										getBody(void) const;
	std::string										getPathW(void) const;
	int												getPort(void) const;
	ServerInfo										getCurrentServer() const;
	const std::map<int, std::filesystem::path>&		getAvailableErrors() const;
	// std::filesystem::path							getPythonPath() const;
	std::string										getContentLength(void) const;

	bool wasExecuted();

	// Setters
	void	setExecuted(bool val);
	void	setHttpRequest(std::string req);
	void	setMethod(std::string meth);
	void	setUrl(std::string url);
	void	setVersion(std::string ver);
	void	setCurrentServer(const ServerInfo& server);

	// Parse
	void	readRequest(const std::string& req, int seg_flag_safe); //throws exception
	void	parseRequestLine(std::string& line, int seg_flag_safe);
	void	parseLine(std::string line);
	void	extractPortFromHost();
	bool	isValid();
	bool	validatePost();
	// bool	validateDelete();
	void	parseMethod(std::string& line);
	void	parseUrl(std::string& line);
	void	parseHttpVersion(std::string& line);
	void	updateFilename();
	bool	isCgi();

	// Debug
	void	printHeaders(void) const;
	void	printRequest(void) const;
	void	printBody(void) const;

	// Execute methodes
	const HttpResponse	performMethod();
	const HttpResponse	postCase(HttpResponse& resp);
	const HttpResponse	getCase(HttpResponse& resp);
	const HttpResponse	deleteCase(HttpResponse& resp);
	// const HttpResponse	cgiCase(int poll_fd, HttpResponse& resp);

	// Utils
	// char **createEnv(std::filesystem::path path_of_program_to_execute);
	// void	executeCgi(std::filesystem::path path_of_program_to_execute, int* fd);

	// Exceptions
	class httpParserException : public std::exception {
		public:
			virtual const char *what() const throw();
	};
};
