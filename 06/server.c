#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buffer[1024];

    FILE *fp = fopen("file.txt", "r");

    if (fp == NULL) {
        perror("file.txt");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9000);

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 5);

    printf("Server waiting for client...\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client, &len);

    printf("Client connected!\n");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

        printf("Sending: %s", buffer);

        send(client_fd, buffer, strlen(buffer), 0);
    }

    printf("\nFile sent successfully!\n");

    fclose(fp);
    close(client_fd);
    close(server_fd);

    return 0;
}