#include "../../inc/sockets_and_poll/PollFdWithFlag.hpp"

PollFdWithFlag::PollFdWithFlag() : 
pollfd_(), 
state_(FIRST_TIME),
type_(NON_SETTED),
method_is_finished_(NO),
final_buffer_({}),
final_resp_buffer_({}),
req_(),
resp_(), 
real_max_body_size_ln_(0),
content_length_(0),
timeout_(0),
connected_fds_(),
connected_server_()
{
};
PollFdWithFlag::PollFdWithFlag(const PollFdWithFlag& other)
: pollfd_(other.pollfd_),
state_(other.state_),
type_(other.type_),
method_is_finished_(other.method_is_finished_),
final_buffer_(other.final_buffer_),
final_resp_buffer_(other.final_resp_buffer_),
req_(other.req_),
resp_(other.resp_),
real_max_body_size_ln_(other.real_max_body_size_ln_),
content_length_(other.content_length_),
timeout_(other.timeout_),
connected_fds_(other.connected_fds_),
connected_server_(other.connected_server_)
{};

PollFdWithFlag::PollFdWithFlag(
	pollfd temp_fd, 
	int state,
	int type,
	int post_is_finished,
	std::string final_buffer,
	std::string final_resp_buffer, 
	HttpRequest req,
	HttpResponse resp,
	size_t real_max_body_size_ln,	
	size_t content_length,
	size_t timeout,
	std::vector<size_t> connected_fds,
	ServerInfo connected_server) :
pollfd_(temp_fd), 
state_(state),
type_(type),
method_is_finished_(post_is_finished),
final_buffer_(final_buffer),
final_resp_buffer_(final_resp_buffer),
req_(req),
resp_(resp),
real_max_body_size_ln_(real_max_body_size_ln),
content_length_(content_length),
timeout_(timeout),
connected_fds_(connected_fds),
connected_server_(connected_server)
{};
PollFdWithFlag& PollFdWithFlag::operator=(const PollFdWithFlag& other)
{
	if (this != &other)
	{
		pollfd_ = other.pollfd_;
		state_ = other.state_;
		type_ = other.type_;
		method_is_finished_ = other.method_is_finished_;
		final_buffer_ = other.final_buffer_;
		final_resp_buffer_ = other.final_resp_buffer_;
		req_ = other.req_;
		resp_ = other.resp_;
		real_max_body_size_ln_ = other.real_max_body_size_ln_;
		content_length_= other.content_length_;
		timeout_ = other.timeout_;
		connected_fds_ = other.connected_fds_;
		connected_server_ = other.connected_server_;
	}
	return *this;
};
PollFdWithFlag::~PollFdWithFlag()
{
};
//setters
void PollFdWithFlag::setFinalBuffer(const std::string& other_final)
{
	final_buffer_ = other_final;
};
void PollFdWithFlag::setPollFd(const pollfd& other_fd)
{
	pollfd_ = other_fd;
};

void PollFdWithFlag::setRequest(const HttpRequest req)
{
	req_ = req;
};
void PollFdWithFlag::setRealMaxBodySizeLn(const size_t& real_max_body_size_ln) // maximum between 1024 and 10000000
{
	real_max_body_size_ln_ = std::min((size_t)10000000,(size_t)std::max((size_t)4096, real_max_body_size_ln));
};

void PollFdWithFlag::setContentLength(int bytes, char buffer[])
{
	if (content_length_ == 0)
	{
		std::string temp = final_buffer_;
		if (bytes > 0)
			for (size_t i = 0; i != (size_t)bytes; i++)
				temp.push_back(buffer[i]);
		HttpRequest r;
		r.readRequest(temp, 0);
		std::unordered_map<std::string, std::string> headers = r.getHeaders();
		if (headers.find("sec-ch-ua-platform") != headers.end() && headers.find("Content-Length") != headers.end())
			content_length_ = std::stoul(headers["Content-Length"]);
	}
}

void PollFdWithFlag::setConnectedFds(const std::vector<size_t>& connected_fds)
{
	connected_fds_ = connected_fds;
};
void PollFdWithFlag::setConnectedServer(const ServerInfo& connected_server)
{
	connected_server_ = connected_server;
};

void PollFdWithFlag::setFinalRespBuffer()
{
	HttpResponse response;
	std::string response_str;
	if (!req_.isCgi() \
		|| (req_.isCgi() && req_.isInvalid()) \
		|| (req_.isCgi() && req_.isForbidden()))
	{
		response = req_.performMethod();
		response_str = response.respond(req_);
	}
	else
	{
		std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(pollfd_.fd);
		if (cgi == nullptr)
		{
			std::cout << RED << "Cgi is not accessible from singleton\n";
			return;
		}
		response_str = cgi->getRespBody();
		std::cout << GREEN << "RESPONSE\n" << response_str << std::endl << QUIT;
	}
	final_resp_buffer_.append(response_str);
}

void PollFdWithFlag::setFinalRespBufferIfCgi()
{
	HttpResponse response;
	if (final_resp_buffer_.empty())
	{
		std::string response_str;

		if (CgiSingleton::getInstance().access_cgi(pollfd_.fd) == nullptr)
		{
			std::cout << YELLOW << "ADDING EVENT:" << pollfd_.fd << QUIT<< std::endl;
			CgiSingleton::getInstance().add_event(pollfd_.fd, std::make_shared<Cgi>(pollfd_.fd, req_));
		}
		std::shared_ptr<Cgi> cgi = CgiSingleton::getInstance().access_cgi(pollfd_.fd);
		if (!cgi->hasForked())
			cgi->execution_close();
		if (cgi->performed_wait())
		{
			req_.setExecuted(true);
			response = cgi->response_of_cgi(response);
			response_str = response.respond(req_);
			cgi->setResponseBody(response_str);
		}
		final_resp_buffer_.append(response_str);
	}
} //tested