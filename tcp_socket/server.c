#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5000
#define BUFFER_SIZE 1024

// Thread function to continuously receive messages from client
void *receive_messages(void *socket_fd) {
    int client_fd = *(int *)socket_fd;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("\n[Client disconnected or connection lost]\n");
            exit(0);
        }

        printf("\nClient: %s", buffer);
        printf("You: ");
        fflush(stdout);
    }
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t recv_thread;
    char message[BUFFER_SIZE];

    // Create TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse port immediately
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");
    printf("Type your message and press Enter. Type 'exit' to quit.\n\n");

    // Create background thread for receiving messages
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&client_fd);

    // Main thread handles sending messages
    while (1) {
        printf("You: ");
        fflush(stdout);
        fgets(message, BUFFER_SIZE, stdin);

        if (strncmp(message, "exit", 4) == 0) {
            break;
        }

        send(client_fd, message, strlen(message), 0);
    }

    close(client_fd);
    close(server_fd);
    printf("Server closed.\n");

    return 0;
}