#pragma once
#include "Cgi.hpp"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <memory>

class Cgi;

class CgiSingleton
{
private:
	static std::unordered_map<int, std::shared_ptr <Cgi>>	running_cgis_;
	CgiSingleton();
	CgiSingleton(const CgiSingleton&) = delete;
	CgiSingleton& operator=(const CgiSingleton&) = delete;
public:
	~CgiSingleton();

	// The static method that controls access to the singleton instance.
	static CgiSingleton& getInstance();

	void add_event(int poll_fd, std::shared_ptr<Cgi> event);
	void remove_event(int poll_fd);
	std::unordered_map<int, std::shared_ptr<Cgi>> getRunningCgis() const;
	std::shared_ptr<Cgi> access_cgi(int poll_fd);
	size_t singleton_size();

};
