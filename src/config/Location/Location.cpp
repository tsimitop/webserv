#include "Location.hpp"

Location::Location() : allowed_methods(-1), location_html(""), uploads_dir(""), uploads_html(""), redir("")
{
	cgi_map["cgi_one"] = "";
	cgi_map["cgi_two"] = "";
	cgi_map["cgi_three"] = "";
};
Location::Location(const Location& other)
{
	uploads_dir = other.uploads_dir;
	location_html = other.location_html;
	uploads_html = other.uploads_html;
	allowed_methods = other.allowed_methods;
	redir = other.redir;
	//-----map of cgis-----------------------
	cgi_map = other.cgi_map;

};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		uploads_dir = other.uploads_dir;
		uploads_html = other.uploads_html;
		allowed_methods = other.allowed_methods;
		redir = other.redir;
		//-----map of cgis-----------------------
		cgi_map = other.cgi_map;
	}
	return *this;
};
Location::~Location()
{
};
//===================Methods======================================================
// void						Location::validPath(std::string value)
// {

// };
// void						Location::validLocation(std::string value)
// {

// };