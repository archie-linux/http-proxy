#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <netdb.h>


const int PROXY_PORT = 8080;
const int THREAD_POOL_SIZE = 10;

std::queue<int> client_sockets;
std::mutex queue_mutex;
std::condition_variable condition;


void proxy_request(int client_socket) {
    const int BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];

    // Read client request
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        std::cerr << "Error reading from client\n";
        close(client_socket);
        return;
    }

    // Extract destination host and port from HTTP request
    std::string request(buffer, bytes_received);
    size_t host_start = request.find("Host: ") + 6;
    size_t host_end = request.find("\r\n", host_start);
    std::string host_port = request.substr(host_start, host_end - host_start);

    // Determine if it's an HTTP request (port 443 or known https protocol)
    bool is_https = host_port.find(":443") != std::string::npos || request.find("CONNECT ") != std::string::npos;

    // Find the hostname and port
    std::string hostname, port_str;
    size_t port_pos = host_port.find(":");
    if (port_pos != std::string::npos) {
        hostname = host_port.substr(0, port_pos);
        port_str = host_port.substr(port_pos + 1);
    }  else {
        hostname = host_port;
        port_str = is_https ? "443" : "80";
    }
    int port = std::stoi(port_str);

    // Resolve hostname to ip address
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return;
    }

    // Create socket for connection to destination server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket to server\n";
        freeaddrinfo(res);
        close(client_socket);
        return;
    }

    // Connect to destination server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = ((struct sockaddr_in *)(res -> ai_addr)) -> sin_addr;
    freeaddrinfo(res);

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to server" << strerror(errno) << std::endl;
        close(server_socket);
        close(client_socket);
        return;
    }

    std::cout << "Request: " << std::endl << std::endl;
    std::cout << buffer << std::endl;

    // Forward client request to destination server
    if (send(server_socket, buffer, bytes_received, 0) != bytes_received) {
        std::cerr << "Error sending request to server\n" << strerror(errno)  << std::endl;
        close(server_socket);
        close(client_socket);
        return;
    }

    // Forward server response to client
    ssize_t bytes_sent;

    while ((bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        std::cout << "Response: " << std::endl << std::endl;
        std::cout << buffer << std::endl;

        bytes_sent = send(client_socket, buffer, bytes_received, 0);

        if (bytes_sent != bytes_received) {
            std::cerr << "Error sending response to client" << strerror(errno) << std::endl;
            close(server_socket);
            close(client_socket);
            return;
        }
    }

    close(server_socket);
    close(client_socket);
    return;
}


void handle_client_thread() {
    while (1) {
        int client_socket;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, []{ return !client_sockets.empty(); });
            client_socket = client_sockets.front();
            client_sockets.pop();
        }
        proxy_request(client_socket);
    }
}


void start_proxy() {
    // Create socket for listening
    int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    if (proxy_socket < 0) {
        std::cerr << "Error creating socket\n";
        return;
    }

    // Bind socket to proxy port
    struct sockaddr_in proxy_addr;
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(PROXY_PORT);

    if (setsockopt(proxy_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options\n";
        return;
    }

    if (bind(proxy_socket, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        std::cerr << "Error binding socket\n";
        close(proxy_socket);
        return;
    }

    // Listen for incoming connections
    if (listen(proxy_socket, 5) < 0) {
        std::cerr << "Error listening for connections\n";
        close(proxy_socket);
        return;
    }

    std::cout << "Proxy server started on port " << PROXY_PORT << std::endl << std::endl;

    // Start worker threads
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        threads.emplace_back(handle_client_thread);
    }

    // Accept and handle incoming connections
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_socket < 0) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        {
           std::lock_guard<std::mutex> lock(queue_mutex);
           client_sockets.push(client_socket); 
        }
        condition.notify_one();
    }
}


int main() {
    start_proxy();
    return 0;
}
