# webserv
![Screenshot 2025-07-07 at 5 10 51 PM](https://github.com/user-attachments/assets/0a25aca9-c414-4f8b-9a3e-ad108ab61006)

**webserv** is a lightweight, high-performance HTTP web server implemented in C++. It aims to replicate essential features of modern web servers such as NGINX. It is the third team project of the 42 core curriculum and we worked on it with [Thomas](https://github.com/ThomasNakas).

## Features

- HTTP/1.1 support
- Static file serving
- CGI execution (Python)
- Configurable via a custom config file
- Basic error handling (404, 500, etc.)
- Support for multiple client connections using `poll()`
- Customizable routing and server blocks

## Getting Started

### Prerequisites

- C++11 or later
- Unix-based OS (Linux or macOS)
- Make

### Building

```bash
git clone https://github.com/tsimitop/webserv.git
cd webserv
make
```
You can build it in docker if you prefer by using the following.
```bash
make docker_run
```


## Run the server
```bash
./webserv [conf_filename.conf]
```
- If a config file is not provided the default will be used. Please provide only filename, only the config directory will be searched.
- Navigate to http://localhost:your_port (our default ports are 4242, 4243, 4244, 4245, 4246, 4247). If you wish to change them and are working with Docker make sure to update the Makefile.

- Our index provide some correct and faulty cgis to test.
- We have multiple www directories with various uploads folders that can be configured differently.
- Play around with:
  - allowed/forbidden methods
  - large/small text uploads
  - deletions (to delete specify deleting directory i.e.->uploads2/file_to_delete.txt)
  - click on cgis
  - checkout the redirections of the config file
- Multiple problematic configs are provided and can be tested with the testing.sh found in /utils.
- Have fun with our webserver!
