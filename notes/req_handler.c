#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8081
#define BUFFER_SIZE 4096

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received < 0) {
        perror("recv");
        close(client_sock);
        return;
    }

    buffer[bytes_received] = '\0';  // Null-terminate the request

    printf("Received Request:\n%s\n", buffer);

    // Extract method, path, and protocol
    char method[10], path[1024], protocol[20];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    printf("HTTP Method: %s\n", method);
    printf("Requested Path: %s\n", path);

    // Prepare response based on path
    char response[BUFFER_SIZE];

    if (strcmp(path, "/hello") == 0) {
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 18\r\n"
            "\r\n"
            "Hello, Custom Page!");
    } else if (strcmp(path, "/bye") == 0) {
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 15\r\n"
            "\r\n"
            "Goodbye, World!");
    } else {
        snprintf(response, sizeof(response),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 14\r\n"
            "\r\n"
            "404 Not Found");
    }

    send(client_sock, response, strlen(response), 0);
    close(client_sock);
}


int main() {
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	// Create socket
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Bind address and port
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	// Listen for connections
	if (listen(server_sock, 10) < 0) {
		perror("listen");
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d...\n", PORT);

	// Accept and handle clients
	while (1) {
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
		if (client_sock < 0) {
			perror("accept");
			continue;
		}

		// Fork a new process for each client
		if (fork() == 0) {
			close(server_sock); // Child doesn't need the listener socket
			handle_client(client_sock);
			exit(0);
		}

		close(client_sock); // Parent closes the client socket
	}

	close(server_sock);
	return 0;
}
