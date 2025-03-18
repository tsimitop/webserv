#include "../../inc/testing/HttpRequestTester.hpp"

int main(void)
{
	std::cout << "Tests with GET\n";
	HttpRequestTester get1("GET /index.html HTTP/1.1\r\n\r\n", "Test get without Host.", "\033[31mShould fail.\033[0m");
	get1.testRequest();
	HttpRequestTester get2("GET /index.html HTTP/1.1\r\n\r\nHost: ", "Test get with empty Host.", "\033[31mShould fail.\033[0m");
	get2.testRequest();
	HttpRequestTester get3("GET /index.html HTTP/1.1\r\n\r\nHost: localhost", "Test get with ok Host but no rnrn.", "\033[31mShould fail.\033[0m");
	get3.testRequest();
	HttpRequestTester get4("GET /index.html HTTP/1.1\r\n\r\nHost: localhost\r\n\r\n", "Test get with ok Host and rnrn", "\033[32mShould pass.\033[0m");
	get4.testRequest();

	std::cout << "Tests with POST\n";
	HttpRequestTester post1("POST /index.html HTTP/1.1\r\n\r\n", "Test post without Host.", "\033[31mShould fail.\033[0m");
	post1.testRequest();
	HttpRequestTester post2("POST /index.html HTTP/1.1\r\n\r\nHost: ", "Test post with empty Host.", "\033[31mShould fail.\033[0m");
	post2.testRequest();
	HttpRequestTester post3("POST /index.html HTTP/1.1\r\n\r\nHost: localhost\r\n\r\n", "Test post with ok Host but no Content-Length or Content-Type", "\033[31mShould fail.\033[0m");
	post3.testRequest();
	HttpRequestTester post4("POST /api/user HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/json\r\nContent-Length: 33\r\nAuthorization: Bearer <your_token>\r\n\r\n", "Test post with ok Host, ok Content-Length and ok Content-Type", "\033[32mShould pass.\033[0m");
	post4.testRequest();

	std::cout << "Tests with DELETE\n";
	HttpRequestTester delete1("DELETE /index.html HTTP/1.1\r\n\r\n", "Test delete without Host.", "\033[31mShould fail.\033[0m");
	delete1.testRequest();
	HttpRequestTester delete2("DELETE /index.html HTTP/1.1\r\n\r\nHost: ", "Test delete with empty Host.", "\033[31mShould fail.\033[0m");
	delete2.testRequest();
	HttpRequestTester delete3("DELETE /index.html HTTP/1.1\r\n\r\nHost: localhost", "Test delete with ok Host but no rnrn.", "\033[31mShould fail.\033[0m");
	delete3.testRequest();
	HttpRequestTester delete4("DELETE /index.html HTTP/1.1\r\n\r\nHost: localhost\r\n\r\n", "Test delete with ok Host but no rnrn.", "\033[32mShould pass.\033[0m");
	delete4.testRequest();

}