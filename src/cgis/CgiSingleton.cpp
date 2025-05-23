#include "../../inc/cgis/CgiSingleton.hpp"
#include <memory>

std::unordered_map<int, std::shared_ptr<Cgi>> CgiSingleton::running_cgis_;

CgiSingleton::CgiSingleton(){}

void CgiSingleton::add_event(int poll_fd, std::shared_ptr<Cgi> event)
{
	running_cgis_.emplace(poll_fd, event);
}

void CgiSingleton::remove_event(int poll_fd)
{
	auto found = running_cgis_.find(poll_fd);
	if (found != running_cgis_.end())
	{
		running_cgis_.erase(found);
	}
}

std::unordered_map<int, std::shared_ptr<Cgi>> CgiSingleton::getRunningCgis() const
{
	if (running_cgis_.size() == 0)
		std::cout << "No running cgis at the moment\n";
	return (running_cgis_);
}

CgiSingleton& CgiSingleton::getInstance()
{
	static CgiSingleton instance;
	return instance;
}

size_t CgiSingleton::singleton_size()
{
	return (running_cgis_.size());
}

std::shared_ptr<Cgi> CgiSingleton::access_cgi(int poll_fd)
{
	auto it = running_cgis_.find(poll_fd);
	if (it != running_cgis_.end())
		return it->second;
	else
		return nullptr;
}

CgiSingleton::~CgiSingleton(){}
