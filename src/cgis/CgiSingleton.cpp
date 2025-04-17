#include "../../inc/cgis/CgiSingleton.hpp"
#include <memory>

std::unordered_map<int, Cgi> CgiSingleton::running_cgis_;

CgiSingleton::CgiSingleton(){}

void CgiSingleton::add_event(int poll_fd, Cgi& event)
{
	running_cgis_.emplace(poll_fd, event);
	std::cout << "Added event.\n";
}

void CgiSingleton::remove_event(int poll_fd)
{
	auto found = running_cgis_.find(poll_fd);
	if (found != running_cgis_.end())
	{
		std::cout << "FD of POLL: " << poll_fd << "-> Removed event.\n";
		running_cgis_.erase(found);
	}
}

std::unordered_map<int, Cgi> CgiSingleton::getRunningCgis() const
{
	if (running_cgis_.size() == 0)
		std::cout << "No running cgis at the moment\n";
	return (running_cgis_);
}

CgiSingleton CgiSingleton::getInstance()
{
	static CgiSingleton instance;
	return instance;
}

// std::shared_ptr<Cgi> CgiSingleton::access_event(int poll_fd) const
// {
// 	return (running_cgis_[poll_fd]);
// }

// std::shared_ptr<Cgi> CgiSingleton::access_event(int poll_fd) const
// {
// 	return (running_cgis_[poll_fd]);
// }


Cgi& CgiSingleton::access_cgi(int poll_fd) const
{
	return (running_cgis_.at(poll_fd));
}

CgiSingleton::~CgiSingleton(){}

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