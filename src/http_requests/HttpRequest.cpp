#include "../../inc/http_requests/HttpRequest.hpp"
#include "../../inc/cgis/Cgi.hpp"

// Orthodox Canonical Class Form
HttpRequest::HttpRequest() : port_(80), current_server_(){}

HttpRequest::HttpRequest(const HttpRequest& other) {*this = other;}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->current_server_ = other.current_server_;
		this->headers_ = other.headers_;
		this->bodyVector_ = other.bodyVector_;
		this->bodyComplete_ = other.bodyComplete_;
		this->httpRequest_ = other.httpRequest_;
		this->method_ = other.method_;
		this->url_ = other.url_;
		this->version_ = other.version_;
		this->port_ = other.port_;
		this->content_length_ = other.content_length_;
		this->basePath_ = other.basePath_;
		this->filename_ = other.filename_;
		this->available_errors_ = other.available_errors_;
		this->executed_ = other.executed_;
		this->current_www_path_ = other.current_www_path_;
		this->is_redir_ = other.is_redir_;
		this->req_is_invalid_ = other.req_is_invalid_;
		this->forbidden_meth_ = other.forbidden_meth_;
	}
	return (*this);
}

HttpRequest::~HttpRequest() {}

// Parameterized constructor
HttpRequest::HttpRequest(const std::string& request, const ServerInfo& server_info)
{
	port_ = 80;
	httpRequest_ = request;
	current_server_ = server_info;
	available_errors_ = this->current_server_.errors;
	current_www_path_ = this->current_server_.www_path_;
	executed_ = false;
	is_redir_ = false;
	req_is_invalid_ = false;
	forbidden_meth_ = false;
	// Test for proper location redirs
	// for (Location& location : this->current_server_.locations_)
	// {
	// 	std::cout << RED << location.name_ << " = locations name\n" << QUIT;
	// 	std::cout << RED << location.redir_status_ << " = locations redir_status_\n" << QUIT;
	// 	std::cout << RED << location.redir_location_ << " = locations redir_location_\n" << QUIT;
	// 	std::cout << RED << std::boolalpha << location.is_redir_ << " = locations is_redir_\n" << QUIT;
	// }
}

bool HttpRequest::wasExecuted()
{return executed_;}

// Getters
bool	 HttpRequest::isInvalid() const
{return req_is_invalid_;}

bool	 HttpRequest::isForbidden() const
{return forbidden_meth_;}

bool HttpRequest::isRedirection() const
{return is_redir_;}

const std::map<int, std::filesystem::path>&	HttpRequest::getAvailableErrors() const
{return available_errors_;}

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

std::string	HttpRequest::getPathW(void) const
{return (current_server_.www_path_);}

int HttpRequest::getPort(void) const
{return (port_);}

std::string HttpRequest::getContentLength(void) const
{return (content_length_);}

ServerInfo	HttpRequest::getCurrentServer() const
{return current_server_;}

// Setters
void	HttpRequest::setCurrentServer(const ServerInfo& server)
{current_server_ = server;}

void	HttpRequest::setExecuted(bool val)
{executed_ = val;}

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

void	HttpRequest::parseRequestLine(std::string& line, int seg_flag_safe)
{
	parseMethod(line);
	parseUrl(line);
	parseHttpVersion(line);
	if (seg_flag_safe)
	{
		std::vector<std::string> allowed =  current_server_.locations_[0].allowed_methods_;
		if (std::find(allowed.begin(), allowed.end(), method_) == allowed.end())
			forbidden_meth_ = true;
	}
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

void	HttpRequest::readRequest(const std::string& req, int seg_flag_safe)
{
	std::string requestLine = req;
	int body = 0;

	std::string	line = requestLine.substr(0, requestLine.find("\r\n"));

	if (!line.empty() && line.size() > 0)
		parseRequestLine(line, seg_flag_safe);
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
	this->extractPortFromHost();
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
				content_length_ = it->second;
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
		{
			req_is_invalid_ = true;
			std::cout << RED << "Can't handle uknown method->400 Bad Request" << QUIT << std::endl;
		}
		if (url_.empty())
		{
			req_is_invalid_ = true;
			std::cout << RED << "No url->server should be closed by foreign host" << QUIT << std::endl;
		}
		if (version_.empty())
		{
			req_is_invalid_ = true;
			std::cout << RED << "No HTTP version->server should be closed by foreign host" << QUIT << std::endl;
		}
		return (false);
	}
	auto it = headers_.begin();
	for (it = headers_.begin(); it != headers_.end(); it++)
		if (it->first == "Host")
			break;
	if (it == headers_.end() || (it->second).empty() || isOnlyWhitespace(it->second))
	{
		std::cout << RED << "Didn't find Host" << QUIT << std::endl;
		req_is_invalid_ = true;
		return (false);
	}
	if (method_ == "POST" && !validatePost())
	{
		req_is_invalid_ = true;
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
	std::string length = headers_["Content-Length"];
	//Thomas additional cond start
	if ((size_t)this->current_server_.client_max_body_size_ < (size_t)stoul(length))
	{
		resp.createResponse(413, available_errors_[413]);
		return resp;
	}
	//Thomas additional cond end
	if ((int)(this->getBody().length()) != stoi(length)) // remove most of this if statement after debugging
	{
		std::cout << RED << "Body length is not correct\n" << QUIT;
		resp.createResponse(500, available_errors_[500]);
	}
	else
	{
		std::ofstream file(current_uploads_path / filename);
		if (!file.is_open()) // probably needs to be handled by html and/or config
			resp.createResponse(500, available_errors_[500]);
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

const HttpResponse	HttpRequest::getCase(HttpResponse& resp)
{
	std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
	std::string current_index = this->current_server_.index;
	if (this->url_ == "/" || this->url_ == current_index || this->url_ == "/" + current_index)
	{
		std::filesystem::path target_path = "src/www/" + current_index;
		std::ifstream input_file(target_path.string());
		if (!input_file.is_open())
			resp.createResponse(404, available_errors_[404]);
		else
		{
			//--------------Thomas addition start----------------------------
			size_t client_max_body_size = this->current_server_.client_max_body_size_;
			size_t server_index_length = findTheSizeOfAgivenFile(target_path);
			if (client_max_body_size < server_index_length)
			{
				resp.createResponse(413, available_errors_[413]);
				return resp;
			}
			//--------------Thomas addition end------------------------------
			resp.createResponse(200, target_path);
		}
	}
	else
	{
		std::filesystem::path	target_path = current_www_path_ += this->url_;
		std::ifstream			input_file(target_path.string());
		if (!input_file)
			resp.createResponse(404, available_errors_[404]);
		else
		{
			if (this->url_.substr(this->url_.find_last_of(".")) == ".ico")
				resp.setContentType("image/vnd");
			resp.createResponse(200, target_path);
		}
	}
	return resp;
}

const HttpResponse	HttpRequest::deleteCase(HttpResponse& resp)
{
	std::map<int, std::filesystem::path> available_errors = this->current_server_.errors;
	std::string temp_filename = decodingHexToAscii(this->filename_);
	std::filesystem::path	path_of_file_to_delete = current_www_path_ / temp_filename;

	std::ifstream file(path_of_file_to_delete);
	if (!file)
		resp.createResponse(404, available_errors_[404]);
	else
	{
		if (
				std::filesystem::is_directory(path_of_file_to_delete) ||
				path_of_file_to_delete.string().find("../") != std::string::npos ||
				path_of_file_to_delete.string().find(".html") != std::string::npos ||
				path_of_file_to_delete.string().find(".mp3") != std::string::npos ||
				path_of_file_to_delete.string().find(".ico") != std::string::npos ||
				path_of_file_to_delete.string().find(".cpp") != std::string::npos ||
				path_of_file_to_delete.string().find(".hpp") != std::string::npos
			)
		{
			resp.createResponse(401, available_errors_[401]);
			return (resp);
		}
		file.close();
		int removed = remove(path_of_file_to_delete.c_str());
		if (removed == 0)
		{
			resp.setStatusCode(200);
			resp.setReasonPhrase(200);
			resp.setContentType("text/plain");
		}
		else if (removed != 0)
			resp.createResponse(500, available_errors_[500]);
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

const HttpResponse	HttpRequest::performMethod()
{
	HttpResponse resp;

	if (this->isInvalid())
		resp.createResponse(400, available_errors_[400]);
	else if (this->isForbidden())
		resp.createResponse(405, available_errors_[405]);
	else if (this->getMethod() == "GET")
	{
		for (Location& location : this->current_server_.locations_)
		{
			if (location.is_redir_ && (location.name_ == url_.substr(url_.find_last_of("/") + 1)) && url_.length() > 0)
			{
				is_redir_ = true;
				resp.redirResponse(location);
				return resp;
			}
		}
		resp = getCase(resp);
	}
	else if (this->getMethod() == "POST")
		resp = postCase(resp);
	else if (this->getMethod() == "DELETE")
		resp = deleteCase(resp);
	return resp;
}
