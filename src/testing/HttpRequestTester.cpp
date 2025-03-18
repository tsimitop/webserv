#include "../../inc/testing/HttpRequestTester.hpp"

HttpRequestTester::HttpRequestTester(){}
HttpRequestTester::~HttpRequestTester(){}

HttpRequestTester::HttpRequestTester(const std::string& request, const std::string& purpose, const std::string& expectedBehaviour)
 : request_(request), purpose_(purpose), expectedBehaviour_(expectedBehaviour){}

const std::string& HttpRequestTester::getPurpose() const {return(purpose_);}
const std::string& HttpRequestTester::getExpectedBehaviour() const {return(expectedBehaviour_);}
void HttpRequestTester::setPurpose(const std::string& purpose) {purpose_ = purpose;}
void HttpRequestTester::setExpectedBehaviour(const std::string& expectedBehaviour) {expectedBehaviour_ = expectedBehaviour;}


void	HttpRequestTester::testRequest()
{
	std::cout << "________________________________________________________________" << std::endl;
	std::cout << YELLOW << "Testing Request: " << request_.getHttpRequest() << QUIT << std::endl;
	std::cout << "Test purpose: " << purpose_ << std::endl;
	std::cout << "Expected result: " << expectedBehaviour_ << std::endl << std::endl;
	int fail = 0;
	try
	{
		request_.readRequest(request_.getHttpRequest());
		if (!request_.isValid())
			throw HttpRequest::httpParserException();
	}
	catch(...)
	{
		fail = 1;
		std::cout << "\033[41m" << "\033[97m"<< "Fail ✖✖✖\033[0m\n" << std::endl;
	}
	if (fail == 0)
	{
		request_.extractPortFromHost();
		std::cout << "\033[48;5;41m" << "\033[97m" << "Pass ✔✔✔\033[0m\n" << std::endl;
	}
}
