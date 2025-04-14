#include "../../inc/http_requests/HttpRequest.hpp"

// Orthodox Canonical Class Form
HttpRequest::HttpRequest() : bodyComplete_(""), httpRequest_(""), method_(""), url_(""), version_(""), port_(80), basePath_(""), filename_("") {}

HttpRequest::HttpRequest(const HttpRequest& other) {*this = other;}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->headers_ = other.headers_;
		this->bodyVector_ = other.bodyVector_;
		this->bodyComplete_ = other.bodyComplete_;
		this->httpRequest_ = other.httpRequest_;
		this->method_ = other.method_;
		this->url_ = other.url_;
		this->version_ = other.version_;
		this->port_ = other.port_;
		this->basePath_ = other.basePath_;
		this->filename_ = other.filename_;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {}

// Parameterized constructor
HttpRequest::HttpRequest(const std::string& request, const ServerInfo server_info)
{
	port_ = 80;
	httpRequest_ = request;
	current_server_ = server_info;
}

// Getters
std::unordered_map<std::string, std::string> HttpRequest::getHeaders(void) const
{return (headers_);}

std::string HttpRequest::getHttpRequest(void) const
{return (httpRequest_);}

std::string HttpRequest::getBody(void) const
{return (bodyComplete_);}

std::string HttpRequest::getMethod(void) const
{return (method_);}

std::string HttpRequest::getUrl(void) const
{return (url_);}

std::string HttpRequest::getVersion(void) const
{return (version_);}

std::string HttpRequest::getBasePath(void) const
{return (basePath_);}

std::string HttpRequest::getFilename(void) const
{return (filename_);}

int HttpRequest::getPort(void) const
{return (port_);}

// Setters
void HttpRequest::setHttpRequest(std::string req)
{httpRequest_ = req;}

void HttpRequest::setMethod(std::string meth)
{method_ = meth;}

void HttpRequest::setUrl(std::string url)
{url_ = url;}

void HttpRequest::setVersion(std::string ver)
{version_ = ver;}

static bool isOnlyWhitespace(const std::string& str)
{
	for (char c : str)
	{
		if (!std::isspace(static_cast<unsigned char>(c)))
			return false;
	}
	return true;
}

// Parse
static std::string removeFirstWord(const std::string &input)
{
	std::string::size_type firstSpace = input.find(' ');
	if (firstSpace == std::string::npos)
		return (input);
	return (input.substr(firstSpace + 1, std::string::npos));
}

static bool	isValidMethod(std::string method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
		return (true);
	return (false);
}

void	HttpRequest::parseMethod(std::string& line)
{
	std::string::size_type	firstSpace;
	std::string				method;

	httpRequest_ = line;
	firstSpace = line.find(' ');
	method = line.substr(0, firstSpace);
	if (isValidMethod(method))
		method_ = method;
	else
		method_ = "UKNOWN";
}

void	HttpRequest::parseUrl(std::string& line)
{
	std::string::size_type	firstSpace;
	std::string				url;

	line = removeFirstWord(line);
	firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
	{
		std::cout << "no url provided\n";
		url_ = "";
	}
	else
		url_ = line.substr(0, firstSpace);
}

void	HttpRequest::parseHttpVersion(std::string& line)
{
	std::string				version;

	version = removeFirstWord(line);
	if (version.size() == 0)
		std::cout << "no version provided\n";
	else
		version_ = version;
}

void	HttpRequest::parseRequestLine(std::string& line)
{
	std::vector<std::string> allowed =  current_server_.locations_[0].allowed_methods_;
	parseMethod(line);
	parseUrl(line);
	parseHttpVersion(line);
	if (std::find(allowed.begin(), allowed.end(), method_) == allowed.end())
		std::cout << RED << "Method not allowed\n" << QUIT;
	else 
		std::cout << GREEN << "Method is allowed\n" << QUIT;
}

void	HttpRequest::parseLine(std::string line)
{
	std::string::size_type	colonPos;
	colonPos = line.find(':');
	if (colonPos != std::string::npos)
	{
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 2, std::string::npos);
		headers_.insert(std::pair<std::string, std::string>(key, value));
	}
}

void	HttpRequest::updateFilename()
{
	auto it = headers_.begin();
	if (method_ == "DELETE")
		filename_ = url_.substr(1);
	for (it = headers_.begin(); it != headers_.end(); it++)
	{
		if (it->first == "Content-Disposition")
		{
			std::string line;
			std::istringstream ss(it->second);
			ss >> line;
			getline(ss, line, '=');
			getline(ss, line);
			if ((line[0] == '\'' || line[0] == '\"') && line.length() > 3)
				line = line.substr(1, line.length() - 2);
			if (!line.empty())
				filename_ = line;
			basePath_ = "./uploads"; // By default, change per config
			break;
		}
	}
}

void	HttpRequest::readRequest(const std::string& req)
{
	std::string requestLine = req;
	int body = 0;
	std::string	line = requestLine.substr(0, requestLine.find("\r\n"));

	if (!line.empty() && line.size() > 0)
		parseRequestLine(line);
	requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
	line = requestLine.substr(0, requestLine.find("\r\n"));
	while ((!line.empty() && line.size() > 0) || (body == 0 && method_ == "POST"))
	{
		parseLine(line);
		requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
		line = requestLine.substr(0, requestLine.find("\r\n"));
		if (line.empty() || line.size() == 0)
		{
			requestLine = requestLine.substr(requestLine.find("\r\n") + 2);
			bodyComplete_ = requestLine;
				body = 1;
		}
		else if (requestLine.find("\r\n") == std::string::npos)
			throw HttpRequest::httpParserException();
	}
	if (method_ == "POST" || method_ == "DELETE")
		updateFilename();
}

static bool isOnlyDigit(const std::string& str)
{
	int i = 0;
	for (char c : str)
	{
		if (i == 0)
		{
			if (std::isspace(static_cast<unsigned char>(c)))
			continue;
		}
		if (!std::isdigit(static_cast<unsigned char>(c)))
			return false;
		i++;
	}
	return true;
}

bool	HttpRequest::validatePost(void) // what about Connection → Optional; defaults to keep-alive in HTTP/1.1.
{
	auto it = headers_.begin();
	int	contType = 0;
	int	contLength = 0;

	for (it = headers_.begin(); it != headers_.end(); it++)
	{
		if (it->second.empty() || isOnlyWhitespace(it->second))
		{
			if (it->first == "Content-Type")
				std::cout << RED << "Content-Type isn't specified" << QUIT << std::endl;
			else if (it->first == "Content-Length")
				std::cout << RED << "Content-Length isn't specified" << QUIT << std::endl;
			return (false);
		}
		if (it->first == "Content-Type")
			contType++;
		else if (it->first == "Content-Length")
		{
			int i;
			try
			{
				i = stoi(it->second);
			}
			catch (const std::invalid_argument& e)
			{
				std::cout << RED << "Content-Length specifier is not an int" << QUIT << std::endl;
				return (false);
			}
			if (i < 0 || !isOnlyDigit(it->second))
			{
				std::cout << RED << "Content-Length is not acceptable: " << it->second << QUIT << std::endl;
				return (false);
			}
			contLength++;
		}
	}
	if (contType == 1 && contLength == 1)
		return (true);
	return (false);
}

bool	HttpRequest::isValid()
{
	if (method_ == "UKNOWN" || url_.empty() || version_.empty())
	{
		if (method_ == "UKNOWN")
			std::cout << RED << "Can't handle uknown method->400 Bad Request" << QUIT << std::endl;
		if (url_.empty())
			std::cout << RED << "No url->server should be closed by foreign host" << QUIT << std::endl;
		if (version_.empty())
			std::cout << RED << "No HTTP version->server should be closed by foreign host" << QUIT << std::endl;
		std::cout << RED << "Remove the above after debugging" << QUIT << std::endl;
		return (false);
	}
	auto it = headers_.begin();
	for (it = headers_.begin(); it != headers_.end(); it++)
		if (it->first == "Host")
			break;
	if (it == headers_.end() || (it->second).empty() || isOnlyWhitespace(it->second))
	{
		std::cout << RED << "Didn't find Host" << QUIT << std::endl;
		return (false);
	}
	if (method_ == "POST" && !validatePost())
	{
		std::cout << RED << "Post could not be validated" << QUIT << std::endl;
		return (false);
	}
	return (true);
}

void	HttpRequest::extractPortFromHost()
{
	auto	it = headers_.begin();
	for (it = headers_.begin(); it != headers_.end(); it++)
		if (it->first == "Host")
			break;

	if (it == headers_.end())
		return ;
	std::string	host = it->second;
	auto		pos = host.find(':');
	if (pos == std::string::npos)
	{
		port_ = 80;
		return ;
	}
	std::string	port = it->second.substr(pos + 1, std::string::npos);
	int			portNbr;
	try {
		portNbr = stoi(port);
	}
	catch (const std::invalid_argument& e) {
		std::cout << RED << "Exception thrown: Couldn't convert port to int" << QUIT << std::endl;
		return;
	}

	if (portNbr > 0 && portNbr < 65536) // NOT SURE ABOUT THIS CHECK!!!
		port_ = portNbr;
	else
		std::cout << RED << "Consider choosing another port instead of PORT:" << portNbr << QUIT << std::endl;
}

// Debug
void	HttpRequest::printRequest(void) const
{
	std::cout << MAGENTA << "Printing request: {" << std::endl;
	std::cout << httpRequest_ << std::endl;
	std::cout << "method:\t\t"<< method_ << std::endl;
	std::cout << "url:\t\t"<< url_ << std::endl;
	std::cout << "version:\t"<< version_ << std::endl;
	std::cout << "PORT:\t\t"<< port_ << "}" << QUIT << std::endl;
	std::cout << std::endl;
}

void	HttpRequest::printHeaders(void) const
{
	std::cout << CYAN << "Printing headers: {" << std::endl;
	for (const auto& pair : headers_)
		std::cout << pair.first << ":\t" << pair.second << "}" << std::endl;;
	if (filename_.size())
		std::cout << "\nFilename = " << filename_ << std::endl;
	if (basePath_.size())
		std::cout << "Base path = " << basePath_ << std::endl;
	std::cout << QUIT << std::endl;
}

void	HttpRequest::printBody(void) const
{
	std::cout << BLUE << "Printing whole body: {" << std::endl;
	std::cout << bodyComplete_ << std::endl;
	std::cout << "Printing body elements: " << std::endl;
	for (auto it = bodyVector_.begin(); it != bodyVector_.end(); it++)
		std::cout << "'" << *it << "' ";
	std::cout << "}" << QUIT << std::endl << std::endl;
}

const char *HttpRequest::httpParserException::what() const throw()
{
	return ("Exception thrown: error while parsing http request.");
}

// Execute methodes
const HttpResponse	HttpRequest::postCase(HttpResponse& resp)
{
	std::string filename = this->filename_.substr(this->filename_.find_last_of("/\\") + 1);
	std::filesystem::path current_uploads_path = this->current_server_.uploads_dir_;
	std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
	std::string length = headers_["Content-Length"];
	if ((int)(this->getBody().length()) != stoi(length)) // remove most of this if statement after debugging
		resp.createResponse(500, available_errors[500]);
	else
	{
		std::ofstream file(current_uploads_path / filename);
		std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
		if (!file.is_open()) // probably needs to be handled by html and/or config
			resp.createResponse(500, available_errors[500]);
		else
		{
			file << this->getBody();
			file.close();
			resp.setStatusCode(200);
			resp.setReasonPhrase(200);
			auto it = this->headers_.begin();
			for (it = this->headers_.begin(); it != this->headers_.end(); it++)
			{
				if (it->first == "Content-Type")
					resp.setContentType(it->second);
				else if (it->first == "Content-Length")
					resp.setContentLength(stoi(it->second));
			}
		}
	}
	return resp;
}

void HttpResponse::createResponse(int status_code, std::filesystem::path file)
{
	std::ifstream input_file(file.string());
	setStatusCode(status_code);
	setReasonPhrase(status_code);
	setContentType("text/html");
	std::stringstream ss;
	ss << input_file.rdbuf();
	input_file.close();
	std::string temp;
	temp = ss.str();
	setContentLength(temp.length());
	setBody(temp);
}

const HttpResponse	HttpRequest::getCase(HttpResponse& resp)
{
	std::filesystem::path current_www_path = this->current_server_.www_path_;
	std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
	std::string current_index = this->current_server_.index;
	if (this->url_ == "/" || this->url_ == current_index || this->url_ == "/" + current_index)
	{
		std::filesystem::path target_path = current_www_path / current_index;
		std::ifstream input_file(target_path.string());
		if (!input_file.is_open())
			resp.createResponse(404, available_errors[404]);
		else
			resp.createResponse(200, target_path);
	}
	else
	{
		std::filesystem::path	target_path = current_www_path / this->url_;
		std::ifstream			input_file(target_path.string());
		if (!input_file)
			resp.createResponse(404, available_errors[404]);
		else
			resp.createResponse(200, target_path);
	}
	return resp;
}

const HttpResponse	HttpRequest::deleteCase(HttpResponse& resp)
{
	std::filesystem::path current_www_path = this->current_server_.www_path_;
	std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
	std::filesystem::path	path_of_file_to_delete = current_www_path / this->filename_;
	std::ifstream file(path_of_file_to_delete);
	if (!file)
		resp.createResponse(404, available_errors[404]);
	else
	{
		file.close();
	
		int removed = remove(path_of_file_to_delete.c_str());
		if (removed == 0)
		{
			resp.setStatusCode(200);
			resp.setReasonPhrase(200);
			resp.setContentType("text/plain");
		}
		else if (removed != 0)
			resp.createResponse(500, available_errors[500]);
	}
	return (resp);
}

bool	HttpRequest::isCgi()
{
	if (url_.find_last_of('.') != std::string::npos)
	{
		std::string temp = url_.substr(url_.find_last_of('.') + 1);
		if (temp == "py")
			return (true);
	}
	return (false);
}

const HttpResponse	HttpRequest::cgiCase(HttpResponse& resp)
{
	//handle timeout with poll?
	std::filesystem::path	www_path = std::filesystem::absolute(__FILE__).parent_path().parent_path() += "/www";
	std::filesystem::path	path_of_program_to_execute = www_path += url_;
	std::string executable = url_.substr(url_.find_last_of('/') + 1);
	std::ifstream file(path_of_program_to_execute);
	int fd[2];
	pipe(fd);
	pid_t pid  = fork();
	int status = 0;
	// pip[0] - the read end of the pipe - is a file descriptor used to read from the pipe (input)
	// pip[1] - the write end of the pipe - is a file descriptor used to write to the pipe (output)
	if (pid == 0)
	{
		char *args[] = {
			const_cast<char *>("/usr/local/bin/python3"),
			const_cast<char *>(path_of_program_to_execute.c_str()),
			NULL}
		;
		std::string script_method = "REQUEST_METHOD=" + this->getMethod();

		std::string name = "NAME=" + this->getBody().substr(this->getBody().find_first_of("=") + 1);
		// std::cout << "name = " << name <<std::endl;
		std::string script_filename = "SCRIPT_FILENAME=" + path_of_program_to_execute.string();
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
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve("/usr/local/bin/python3", args, envp);
		exit (EXIT_FAILURE);
	}
	close(fd[1]);
	waitpid(-1, &status, 0);
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	else
		status = EXIT_FAILURE;
	if (status == 0)
	{
		char buffer[4096] = {0}; //BUFFLEN FROM SOCKETS init to null directly
		read(fd[0], buffer, 4096);
		close(fd[0]);
		resp.setStatusCode(200);
		resp.setReasonPhrase(200);
		resp.setContentType("text/html");
		std::string body = "<!DOCTYPE html>\n<html>\n<body>\n<p>";
		body += buffer;
		body += "</p>\n</body>\n</html>";
		resp.setContentLength(body.size());
		resp.setBody(body);
	}
	else
	{
		close(fd[0]);
		std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
		resp.createResponse(500, available_errors[500]);
	}
	return (resp);
}

const HttpResponse	HttpRequest::performMethod()
{
	HttpResponse resp;

	if (isCgi())
	{
		resp = cgiCase(resp);
	}
	else if (this->getMethod() == "GET" && !isCgi())
	{
		resp = getCase(resp);
	}
	else if (this->getMethod() == "POST" && !isCgi())
	{
		resp = postCase(resp);
	}
	else if (this->getMethod() == "DELETE")
	{
		resp = deleteCase(resp);
	}
	return resp;
}
