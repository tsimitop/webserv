#include "../../inc/cgis/Cgi.hpp"
#include "vector"
#include <signal.h>

Cgi::Cgi(const Cgi& other)
: available_errors_(other.available_errors_),
status_(other.status_),
poll_fd_(other.poll_fd_),
cgi_is_executable_(other.cgi_is_executable_),
timed_out_(other.timed_out_),
has_forked_(other.has_forked_),
cgi_request_(other.cgi_request_),  // <--- directly copy-construct
cgi_response_(other.cgi_response_),
exec_complete_(other.exec_complete_),
www_path_(other.www_path_),
url_(other.url_),
path_of_program_to_execute_(other.path_of_program_to_execute_),
executable_(other.executable_),
pid_(other.pid_),
response_body_(other.response_body_),
timeout_total_(other.timeout_total_),
procces_start_(other.procces_start_)
{
pipe_fd_[0] = other.pipe_fd_[0];
pipe_fd_[1] = other.pipe_fd_[1];
}

Cgi& Cgi::operator=(const Cgi& other)
{
	this->available_errors_ = other.available_errors_;
	this->status_ = other.status_;
	this->pipe_fd_[0] = other.pipe_fd_[0];
	this->pipe_fd_[1] = other.pipe_fd_[1];
	this->poll_fd_ = other.poll_fd_;
	this->cgi_is_executable_ = other.cgi_is_executable_;
	this->timed_out_ = other.timed_out_;
	this->cgi_request_ = other.cgi_request_;
	this->cgi_response_ = other.cgi_response_;
	this->www_path_ = other.www_path_;
	this->url_ = other.url_;
	this->path_of_program_to_execute_ = other.path_of_program_to_execute_;
	this->executable_ = other.executable_;
	this->pid_ = other.pid_;
	this->response_body_ = other.response_body_;
	this->procces_start_ = other.procces_start_;
	this->exec_complete_ = other.exec_complete_;
	this->has_forked_ = other.has_forked_;
	return *this;
}

void Cgi::check_timeout()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> current_time_ = std::chrono::high_resolution_clock::now();
	if (procces_start_ + timeout_total_ <= current_time_)
	{
		timed_out_ = true;
		exec_complete_ = true;
		kill(pid_, SIGKILL);
	}
}

Cgi::Cgi(int poll_fd, const HttpRequest& request)
: status_(0), poll_fd_(poll_fd), cgi_is_executable_(true), timed_out_(false), cgi_request_(request), exec_complete_(false)
{
	exec_complete_ = false;
	cgi_is_executable_ = false;
	timed_out_ = false;
	has_forked_ = false;
	has_forked_ = false;
	available_errors_ = cgi_request_.getAvailableErrors();
	www_path_ = request.getPathW();
	url_ = request.getUrl();
	path_of_program_to_execute_ = www_path_ += url_;
	executable_ = url_.substr(url_.find_last_of('/') + 1);
	timeout_total_ = std::chrono::milliseconds(request.getCurrentServer().server_timeout_);

	if (pipe(pipe_fd_) == -1)
		std::cout << "Error creating pipe\n"; // Change that!
	procces_start_ = std::chrono::high_resolution_clock::now();
}

void Cgi::execute()
{
	std::vector<char *> args;
	// language_ = "/usr/local/bin/python3";
	language_ = "/usr/bin/python3"; // DOCKER
	args.push_back(const_cast<char *>(language_.c_str()));
	args.push_back(const_cast<char *>(path_of_program_to_execute_.c_str()));
	args.push_back(nullptr);
	
	std::string script_method = "REQUEST_METHOD=" + cgi_request_.getMethod();

	std::string name = "NAME=" + cgi_request_.getBody().substr(cgi_request_.getBody().find_first_of("=") + 1); // do not proccess here, do it in the .py
	std::string content_length = "CONTENT_LENGTH=" + cgi_request_.getContentLength(); //change to custom
	std::vector<char *> envp;
	envp.push_back(const_cast<char*>(script_method.c_str()));
	envp.push_back(const_cast<char*>(content_length.c_str()));
	envp.push_back(const_cast<char*>(name.c_str()));
	envp.push_back(nullptr);
	int null_fd = open("/dev/null", O_WRONLY);
	if (!null_fd)
		std::cout << "failed to create fd\n";
	dup2(null_fd, STDERR_FILENO);
	close(null_fd);
	dup2(pipe_fd_[1], STDOUT_FILENO);
	dup2(pipe_fd_[0], STDIN_FILENO);
	close(pipe_fd_[0]);
	close(pipe_fd_[1]);
	unsigned char resp = execve(language_.c_str(), args.data(), envp.data());
	exit(resp);
}

bool Cgi::cgiPidDone() const
{return exec_complete_;}

int Cgi::getPollFd() const
{return poll_fd_;}

HttpRequest Cgi::getCgiRequest() const
{return cgi_request_;}

pid_t Cgi::getPid() const
{return pid_;}

int Cgi::getStatus() const
{return status_;}

int Cgi::getFdOne() const
{return pipe_fd_[1];}

void Cgi::setStatus(int status)
{status_ = status;}

std::string Cgi::getRespBody() const
{return response_body_;}

bool Cgi::isExecutable() const
{return cgi_is_executable_;}

bool Cgi::hasTimedOut() const
{return timed_out_;}

bool Cgi::hasForked() const
{return has_forked_;}

Cgi::~Cgi(){}

bool Cgi::performed_wait()
{
	int ret = waitpid(pid_, &status_, WNOHANG);
	if (ret == -1)
		cgi_is_executable_ = false;
	else if (ret == 0)
		check_timeout();
	else if (ret == pid_)
	{
		exec_complete_ = true;
		if (WIFEXITED(status_))
			status_ = WEXITSTATUS(status_);
		else
			status_ = EXIT_FAILURE;
	}
	if (hasTimedOut() == true || exec_complete_ == true)
		return (true);
	return (false);
}

bool Cgi::read_pipe()
{
	if (status_ == 0)
	{
		char buffer[4096] = {0};
		int read_bytes = read(pipe_fd_[0], buffer, 4096);
		if (read_bytes == 0)
			return false;
		if (read_bytes < 0)
		{
			close(pipe_fd_[0]);
			return false;
		}
		while(read_bytes)
		{
			response_body_ += buffer;
			memset(buffer, 0, 4096);
			read_bytes = read(pipe_fd_[0], buffer, 4096);
			if (read_bytes < 0)
			{
				close(pipe_fd_[0]);
				return false;
			}
		}
	}
	close(pipe_fd_[0]);
	return true;
}

HttpResponse Cgi::response_of_cgi(HttpResponse& resp)
{
	if (this->hasTimedOut() == true)
	{
		std::cout<<YELLOW << "Timeout\n" << QUIT;
		resp.createResponse(504, available_errors_[504]);
		return resp;
	}
	if (this->getStatus() != 0)
	{
		std::cout<<YELLOW << "Proccess exit number = " << this->getStatus() << "\n" << QUIT;
		if (this->getStatus() == 2)
			resp.createResponse(404, available_errors_[404]);
		else
			resp.createResponse(500, available_errors_[500]);
		return resp;
	}
	if(this->read_pipe())
		resp.createCgiResponse(200, this->getRespBody());
	else
		resp.createResponse(500, available_errors_[500]);
	return (resp);
}

void Cgi::execution_close()
{
	pid_ = fork();
	if (pid_ == -1)
		std::cout << "Error forking\n"; // Change that!
	has_forked_ = true;
	if (this->getPid() == 0)
		this->execute();
	close(this->getFdOne());
}

void Cgi::setResponseBody(std::string response_str)
{response_body_ = response_str;}
