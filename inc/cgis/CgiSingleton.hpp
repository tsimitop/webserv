#pragma once
#include "Cgi.hpp"
#include <algorithm>
#include <unordered_map>
#include <iostream>

class Cgi;

class CgiSingleton
{
private:
	static std::unordered_map<int, Cgi>	running_cgis_;
	CgiSingleton();
public:
	static void add_event(int poll_fd, Cgi& event);
	static void remove_event(int poll_fd);
	~CgiSingleton();

	// The static method that controls access to the singleton instance.
	static CgiSingleton getInstance();
	std::unordered_map<int, Cgi> getRunningCgis() const;

};


/*
 Solution
All implementations of the Singleton have these two steps in common:

Make the default constructor private, to prevent other objects from using the 
new operator with the Singleton class.
Create a static creation method that acts as a constructor. Under the hood, 
this method calls the private constructor to create an object and saves it in a 
static field. All following calls to this method return the cached object.
If your code has access to the Singleton class, then it’s able to call the 
Singleton’s static method. So whenever that method is called, the same object 
is always returned.
*/