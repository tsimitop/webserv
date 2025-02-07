#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 10
#define BUFFER_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    recv(client_sock, buffer, BUFFER_SIZE, 0);
    
    const char* response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 39\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";
    send(client_sock, response, strlen(response), 0);
    close(client_sock);
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
std::cout << "server_socket: " << server_sock << std::endl;
// std::cout << << std::endl;
    // struct sockaddr_in server_addr{};
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    if (listen(server_sock, BACKLOG) < 0) {
        perror("Listen failed");
        return 1;
    }
    
    std::cout << "Server started on port " << PORT << "..." << std::endl;
    
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        
        if (fork() == 0) {
            close(server_sock);
            handle_client(client_sock);
            return 0;
        }
        close(client_sock);
    }
    close(server_sock);
    return 0;
}

// _______________________________CODECRAFTERS_____________________________
// #include <iostream>
// #include <cstdlib>
// #include <string>
// #include <cstring>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netdb.h>

// int main(int argc, char **argv) {
//   // Flush after every std::cout / std::cerr
//   std::cout << std::unitbuf;
//   std::cerr << std::unitbuf;
  
//   // You can use print statements as follows for debugging, they'll be visible when running tests.
//   std::cout << "Logs from your program will appear here!\n";

//   // Uncomment this block to pass the first stage
//   //
//   int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//   if (server_fd < 0) {
//    std::cerr << "Failed to create server socket\n";
//    return 1;
//   }
  
//   // Since the tester restarts your program quite often, setting SO_REUSEADDR
//   // ensures that we don't run into 'Address already in use' errors
//   int reuse = 1;
//   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
//     std::cerr << "setsockopt failed\n";
//     return 1;
//   }
  
//   struct sockaddr_in server_addr;
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_addr.s_addr = INADDR_ANY;
//   server_addr.sin_port = htons(4221);
  
//   if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
//     std::cerr << "Failed to bind to port 4221\n";
//     return 1;
//   }
  
//   int connection_backlog = 5;
//   if (listen(server_fd, connection_backlog) != 0) {
//     std::cerr << "listen failed\n";
//     return 1;
//   }
  
//   struct sockaddr_in client_addr;
//   int client_addr_len = sizeof(client_addr);
  
//   std::cout << "Waiting for a client to connect...\n";
  
//   int fd_of_accepted_socket = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
//   if (fd_of_accepted_socket < 0)
//   {
//     std::cout << "accept() did not return an fd\n";
//     return 1;
//   }
//   std::cout << "Client connected\n";

//   char buffer[1024] = {};
//   ssize_t  recived = recv(fd_of_accepted_socket, buffer, strlen(buffer), 0);
//   if (recived < 0)
//   {
//     std::cout << "recv() had an error\n";
//     return 1;
//   }
//   std::string request(buffer);
//   std::string message;
//   if (request.find("GET / ") == 0) // || request.find("GET /index.html") == 0
//       message = "HTTP/1.1 200 OK\r\n\r\n";
//   else
//       message = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
//   size_t sent = send(fd_of_accepted_socket, message.c_str(), message.length(), MSG_EOR);
//   if (sent < 0)
//   {
//     std::cout << "send() didn't manage to send bytes\n";
//     return 1;
//   }
//   else
//     std::cout << "send() sent " << sent << " bytes\n";
//   close(server_fd);

//   return 0;
// }
