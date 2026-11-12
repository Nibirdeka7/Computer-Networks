#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client1, client2;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    listen(server_fd, 2);
    printf("Server is waiting for clients...\n");

    // Accept first client
    client1 = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Client 1 connected.\n");

    // Accept second client
    client2 = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Client 2 connected.\n");

    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(client1, &readfds);
        FD_SET(client2, &readfds);

        int maxfd = (client1 > client2) ? client1 : client2;

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        // Message from Client 1
        if (FD_ISSET(client1, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            bytesRead = read(client1, buffer, BUFFER_SIZE);

            if (bytesRead <= 0) {
                printf("Client 1 disconnected.\n");
                break;
            }

            printf("Client 1: %s", buffer);
            send(client2, buffer, strlen(buffer), 0);
        }

        // Message from Client 2
        if (FD_ISSET(client2, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            bytesRead = read(client2, buffer, BUFFER_SIZE);

            if (bytesRead <= 0) {
                printf("Client 2 disconnected.\n");
                break;
            }

            printf("Client 2: %s", buffer);
            send(client1, buffer, strlen(buffer), 0);
        }
    }

    close(client1);
    close(client2);
    close(server_fd);

    return 0;
}