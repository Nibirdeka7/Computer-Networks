#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8090
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    char command[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // Listen for clients
    listen(server_fd, 5);

    printf("Server waiting for client...\n");

    while (1) {

        client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &addr_len);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("\nClient connected.\n");

        memset(command, 0, sizeof(command));

        int n = recv(client_fd, command,
                     sizeof(command) - 1, 0);

        if (n <= 0) {
            close(client_fd);
            continue;
        }

        command[n] = '\0';
        command[strcspn(command, "\r\n")] = '\0';

        printf("Executing command: %s\n", command);

        // Execute the received command
        FILE *fp = popen(command, "r");

        if (fp == NULL) {
            char *msg = "Command execution failed.\n";
            send(client_fd, msg, strlen(msg), 0);
        }
        else {
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                send(client_fd, buffer, strlen(buffer), 0);
            }

            pclose(fp);
        }

        printf("Command execution completed.\n");

        close(client_fd);
    }

    close(server_fd);

    return 0;
}