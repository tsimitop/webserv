#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define QUIT "\033[0m"

enum answer{YES = 1, NO = 0};
enum methods{GET = 1, POST = 2, DELETE = 4};
const std::map<int, std::string> all_posible_errors 
= {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{103, "Early Hints"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	{404, "error on Wikimedia"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{422, "Unprocessable Content"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{507, "Insufficient Storage"},
	{511, "Network Authentication Required"}
};
struct Location
{
	//===============ATTRIBUTES =====================================================
	//------------------lines of locations-----
	std::vector<std::string>	location_lines_;
	//------------------Executable path--------
	std::filesystem::path 		executable_root_location_;
	//-------flags-----------------------------
	int							valid_location_;
	//-------others-----------------------------
	long long					client_max_body_size_;
	std::vector<std::string>	allowed_methods_;
	std::filesystem::path 		location_html_;
	std::filesystem::path 		uploads_dir_;
	std::filesystem::path 		uploads_html_;
	std::filesystem::path 		redir_;
	std::map <std::string, std::filesystem::path> cgi_map_;
	//-------flags-----------------------------
	//===============DEFAULT CONSTRUCTORS ===========================================
	Location();
	Location(std::filesystem::path absolute_path);
	Location(const Location& other);
	Location& operator=(const Location& other);
	~Location();
	//===============METHODS ========================================================
	//--------validators-----------------------
	void						validPath(std::string line);
	void						validClientMaxBodySize(std::string& value);
	void						validMethods(std::string line);
	int							validLocation();
	//--------setters--------------------------
	void						settingTheRightPath(std::string value, std::filesystem::path& p);
	void						setClientMaxBodySize(std::string line);
	void						setAllowedMethods(std::string line);
	void						setPath (std::string line, std::filesystem::path& attribute);
	void						pushCgiMap(std::string line);
};
//===================OUTER FUNCTIONS ================================================
//----------Fuction that needs to bedefined----
//- and being used form all the other Structs--
std::string 					spaceTrimmer(std::string str);
int								countWords(std::string line);
int								strIsNumber(std::string str);
int								strIsAlphaOr(std::string str, char extraChar);
void							printError(std::string type, std::string line);

template <typename T>
	std::ostream& operator<<(std::ostream& os, std::vector<T>& vec)
	{
		os << "[\n";
		for (T t : vec)
			os << t << std::endl;
		os << "]\n";
		return os;
	}