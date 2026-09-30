#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5001
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    char buffer[BUFFER_SIZE];
    char input_message[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Create a UDP socket (SOCK_DGRAM)
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Type a string to send to the UDP server.\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("Enter string: ");
        fflush(stdout);
        
        if (fgets(input_message, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        if (strncmp(input_message, "exit", 4) == 0) {
            break;
        }

        // Send string to server
        sendto(client_fd, input_message, strlen(input_message), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));

        memset(buffer, 0, BUFFER_SIZE);

        // Receive reversed string from server
        int bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&server_addr, &addr_len);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Server response: %s\n\n", buffer);
        }
    }

    close(client_fd);
    printf("Client closed.\n");

    return 0;
}