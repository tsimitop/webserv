#include "CgiSingleton.hpp"

void CgiSingleton::add_event(Cgi event)
{
	running_cgis_.push_back(event);
	std::cout << "Added enent.\n";
}

void CgiSingleton::remove_event(int poll_fd)
{
	for (auto it = running_cgis_.begin(); it != running_cgis_.end(); it++)
	{
		if ((*it).getPollFd() == poll_fd)
		{
			running_cgis_.erase(it);
			std::cout << "FD of POLL: " << poll_fd << ": Removed event.\n";
			break;
		}
	}
}

std::vector<Cgi> CgiSingleton::getInstance()
{
	if (running_cgis_.size() == 0)
		std::cout << "No running cgis at the moment\n";
	else
		return (running_cgis_);
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