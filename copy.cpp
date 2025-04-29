#include "inc/http_requests/HttpRequest.hpp"
#include "inc/http_requests/HttpResponse.hpp"
#include "inc/config/Http.hpp"
#include "inc/sockets_and_poll/Poll.hpp"

int	Poll::pollin(size_t i)
{
	// std::cout << "Entered pollin\n";

	int answer = YES;
	bool is_pollin = fds_with_flag_[i].pollfd_.revents & (POLLIN);
	int revent = fds_with_flag_[i].pollfd_.revents;
	(void) revent;
	(void) is_pollin;
	if (fds_with_flag_[i].pollfd_.revents & (POLLIN))
	{
	std::cout << YELLOW << "POLLIN revent found\n" << QUIT;
	
		size_t temp_len = lengthProt(i);
		char buffer[lengthProt(i) + 1];
		
		memset(buffer, 0, lengthProt(i) + 1); //
		int bytes = recv(fds_with_flag_[i].pollfd_.fd, buffer, temp_len, 0);
		std::cout << MAGENTA << buffer << QUIT << std::endl;
		if (bytes == 0 || bytes < 0)
			answer = eAgainAndEWouldblock(i, bytes);
		else
		{
			int checking_signals = checkingForSignals(buffer, bytes, fds_with_flag_[i].final_buffer_);
			if (checking_signals == SIG)
				return SIG;
			std::string temp_buffer;
			temp_buffer = buffer;
			bool is_post = fds_with_flag_[i].final_buffer_.find("POST ", 0, 4) != std::string::npos;
			bool rnrn_found = fds_with_flag_[i].final_buffer_.find("\r\n\r\n") != std::string::npos;
			std::string temp;
			std::cout << fds_with_flag_[i].final_buffer_ << " = fds_with_flag_[i].final_buffer_\n";
			if (fds_with_flag_[i].final_buffer_.find("\r\n\r\n") != std::string::npos)
			{
				std::cout << "UPDATING temp\n";
				temp = fds_with_flag_[i].final_buffer_.substr(fds_with_flag_[i].final_buffer_.find("\r\n\r\n"));
			}
			std::cout << YELLOW << "temp = [" << temp << "]\n" << QUIT;
			bool rn_found = false;
			std::cout << temp.size() << " = temp.size()\n";
			if (is_post && rnrn_found && temp.size() > 0)
			{
				rn_found = temp.find("\r\n") != std::string::npos;
				std::cout << temp << " = temp\n";
			}
			if (rnrn_found == NO)
			{
				fds_with_flag_[i].final_buffer_.append(temp_buffer);
				rnrn_found = fds_with_flag_[i].final_buffer_.find("\r\n\r\n") != std::string::npos;
				is_post = fds_with_flag_[i].final_buffer_.find("POST ", 0, 4) != std::string::npos;
				std::cout << fds_with_flag_[i].final_buffer_.find("\r\n\r\n") << " = final_buffer.find('\r\n\r\n')\n";
				std::cout << std::boolalpha << is_post << " = is post\n";
				std::cout << std::boolalpha << rnrn_found << " = is rnrn_found\n";
				if (fds_with_flag_[i].final_buffer_.find("sec-ch-ua: ") != std::string::npos)
					definingRequest(i);
				std::cout << rnrn_found << " = rnrn_found\t" << is_post << " = is_post\t" << rn_found << " = rn_found\t\n";
				if ((rnrn_found && is_post == NO) || rn_found)
				{
					std::cout << YELLOW << "rnrn_found && is_post == NO" << QUIT << std::endl;
					definingRequest(i);
	std::cout << "YES1\n";
					return YES;
				}
				std::cout << "WILL CONTINUE\n";
	std::cout << "NO1\n";
				return NO;
			}
			else
			{
				if (is_post == YES && temp_buffer.find("\r\n") == std::string::npos)
				{
					fds_with_flag_[i].final_buffer_.append(temp_buffer);
				definingRequest(i);

	std::cout << "NO2\n";
					return NO;
				}
				fds_with_flag_[i].final_buffer_.append(temp_buffer);
				definingRequest(i);
	std::cout << "YES2\n";
				return YES;
			}
		}
	}
	return answer;
};
