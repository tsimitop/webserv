#pragma once
#include <iostream>
#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/http_requests/HttpResponse.hpp"

class HttpRequestTester
{
private:
	HttpRequest	request_;
	std::string purpose_;
	std::string expectedBehaviour_;

	HttpRequestTester();
	HttpRequestTester(const HttpRequestTester& tester) = delete;
	HttpRequestTester& operator=(const HttpRequestTester& tester) = delete;

public:
	HttpRequestTester(const std::string& request, const std::string& purpose, const std::string& expectedBehaviour);
	~HttpRequestTester();

	const std::string& getPurpose() const;
	const std::string& getExpectedBehaviour() const;

	void setPurpose(const std::string& purpose);
	void setExpectedBehaviour(const std::string& expectedBehaviour);

	void	testRequest();
};
