#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5000
#define BUFFER_SIZE 1024

// Thread function to continuously receive messages from server
void *receive_messages(void *socket_fd) {
    int server_fd = *(int *)socket_fd;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(server_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            printf("\n[Server disconnected or connection lost]\n");
            exit(0);
        }

        printf("\nServer: %s", buffer);
        printf("You: ");
        fflush(stdout);
    }
    return NULL;
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    pthread_t recv_thread;
    char message[BUFFER_SIZE];

    // Create TCP socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configure server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");
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
    printf("Client closed.\n");

    return 0;
}