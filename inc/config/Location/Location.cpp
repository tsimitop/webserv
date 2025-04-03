#include "Location.hpp"

Location::Location() : allowed_methods(-1), dir_path(""), page_path("")
{
};
Location::Location(const Location& other)
{
	dir_path = other.dir_path;
	page_path = other.page_path;
	allowed_methods = other.allowed_methods;
};
Location& Location::operator=(const Location& other)
{
	if (this != &other)
	{
		dir_path = other.dir_path;
		page_path = other.page_path;
		allowed_methods = other.allowed_methods;
	}
	return *this;
};
Location::~Location()
{
};