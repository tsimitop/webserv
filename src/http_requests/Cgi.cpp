#include "Cgi.hpp"

Cgi::Cgi(int poll_fd, const HttpRequest& request)
: status_(0), poll_fd_(poll_fd), cgi_is_executable_(true), timed_out_(false), cgi_request_(request)
{
	www_path_ = request.getPathW();
	url_ = request.getUrl();
	path_of_program_to_execute_ = www_path_ += url_;
	executable_ = url_.substr(url_.find_last_of('/') + 1);

	if (pipe(pipe_fd_) == -1)
		std::cout << "Error creating pipe\n"; // Change that!
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

Cgi::~Cgi(){}
