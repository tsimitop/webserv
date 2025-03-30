#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define PORT "8080"
#define BUFF_LEN 4096

void	start_server();
