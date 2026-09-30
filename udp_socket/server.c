#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5001
#define BUFFER_SIZE 1024

// Helper function to reverse a string in-place
void reverse_string(char *str) {
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    
    // Remove trailing newline character if present from fgets/input
    if (end >= 0 && str[end] == '\n') {
        str[end] = '\0';
        end--;
    }

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int main() {
    int server_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a UDP socket (SOCK_DGRAM)
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive data and capture the sender's (client's) address
        int bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Recvfrom failed");
            continue;
        }

        buffer[bytes_received] = '\0';
        printf("Received: %s", buffer);

        // Reverse the received string
        reverse_string(buffer);

        // Send reversed string back to client
        sendto(server_fd, buffer, strlen(buffer), 0,
               (const struct sockaddr *)&client_addr, addr_len);
        
        printf("Sent back: %s\n\n", buffer);
    }

    close(server_fd);
    return 0;
}