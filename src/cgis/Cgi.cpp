#include "../../inc/cgis/Cgi.hpp"

Cgi::Cgi(const Cgi& other)
{*this = other;}

Cgi& Cgi::operator=(const Cgi& other)
{
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
	return *this;
}

void Cgi::check_timeout()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> current_time_ = std::chrono::high_resolution_clock::now();
	if (procces_start_ + timeout_total_ >= current_time_)
		timed_out_ = true;
}

Cgi::Cgi(int poll_fd, const HttpRequest& request)
: status_(0), poll_fd_(poll_fd), cgi_is_executable_(true), timed_out_(false), cgi_request_(request), exec_complete_(false),
{
	www_path_ = request.getPathW();
	url_ = request.getUrl();
	path_of_program_to_execute_ = www_path_ += url_;
	executable_ = url_.substr(url_.find_last_of('/') + 1);
	timeout_total_ = std::chrono::milliseconds(request.getCurrentServer().server_timeout_);

	// std::cout << (timeout_procces_) <<std::endl;
	if (pipe(pipe_fd_) == -1)
		std::cout << "Error creating pipe\n"; // Change that!
	procces_start_ = std::chrono::high_resolution_clock::now();
	pid_  = fork();
	if (pid_ == -1)
		std::cout << "Error forking\n"; // Change that!
}

void Cgi::execute()
{
	char *args[] = {
		const_cast<char *>("/usr/local/bin/python3"), // from config file
		const_cast<char *>(path_of_program_to_execute_.c_str()),
		NULL}
	;
	std::string script_method = "REQUEST_METHOD=" + cgi_request_.getMethod();

	std::string name = "NAME=" + cgi_request_.getBody().substr(cgi_request_.getBody().find_first_of("=") + 1); // do not proccess here, do it in the .py
	// std::cout << "name = " << name <<std::endl;
	std::string script_filename = "SCRIPT_FILENAME=" + path_of_program_to_execute_.string();
	std::string content_length = "CONTENT_LENGTH=7"; //change to custom
	char* envp[] = {
		const_cast<char*>(script_method.c_str()),
		const_cast<char*>(content_length.c_str()),
		const_cast<char*>(script_filename.c_str()),
		const_cast<char*>(name.c_str()),
		NULL
	};
	int null_fd = open("/dev/null", O_WRONLY);
	if (!null_fd)
		std::cout << "failed to create fd\n";
	dup2(null_fd, STDERR_FILENO);
	close(null_fd);
	dup2(pipe_fd_[1], STDOUT_FILENO);
	dup2(pipe_fd_[0], STDIN_FILENO);
	close(pipe_fd_[0]);
	close(pipe_fd_[1]);
	execve("/usr/local/bin/python3", args, envp);
	exit (EXIT_FAILURE);
	// executeCgi(path_of_program_to_execute, fd);
}

int Cgi::getPollFd() const
{return poll_fd_;}

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

Cgi::~Cgi(){}

bool Cgi::performed_wait()
{
	// should return true if process is done, false, if not done yet.
	// DONE means: 
		// 1- Process returns any status 0 or non-zero (either success or fail)
		// 2- TIME OUT
			// In case of Time out, KILL THE PROCESS, and write timeout status to response object.
	// 1- check if process is done
	// 2- if process is done, pipe out and store results in response object
	int ret = waitpid(pid_, &status_, WNOHANG);
	if (ret == -1)
	{
		cgi_is_executable_ = false;
		std::cout<<"Error in waitpid\n";
	}
	else if (ret == 0)
	{
		check_timeout();
		std::cout<<"Process not finished\n";
	}
	else if (ret > 0)
	{
		exec_complete_ = true;
		std::cout<<"Process finished\n";
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
		{
			std::cout << "Pipe seems to be empty\n";
			return false;
		}
		if (read_bytes < 0)
		{
			std::cout << "Error reading from pipe\n";
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
				std::cout << "Error reading from pipe\n";
				close(pipe_fd_[0]);
				return false;
			}
		}
	}
	close(pipe_fd_[0]);
	return true;
}
