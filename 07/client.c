#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8090
#define BUFFER_SIZE 4096

int main() {

    int sock;
    struct sockaddr_in server_addr;

    char command[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to remote server.\n");

    // Read command
    printf("Enter command: ");

    if (fgets(command, sizeof(command), stdin) == NULL) {
        close(sock);
        return 1;
    }

    // Send command
    send(sock, command, strlen(command), 0);

    // No more data to send
    shutdown(sock, SHUT_WR);

    printf("\nRemote Output:\n");

    // Receive output
    int n;

    while ((n = recv(sock, buffer,
                     sizeof(buffer) - 1, 0)) > 0) {

        buffer[n] = '\0';
        printf("%s", buffer);
    }

    printf("\n");

    close(sock);

    return 0;
}