
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char response[4096];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8090);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 5);

    printf("Web Server is running on port 8080...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer) - 1);

        printf("\nClient Request:\n%s\n", buffer);

        FILE *fp = fopen("index.html", "r");

        if (fp != NULL) {
            int n = fread(response, 1, sizeof(response), fp);
            fclose(fp);

            char header[256];

            sprintf(header,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: close\r\n\r\n", n);

            write(client_fd, header, strlen(header));
            write(client_fd, response, n);

            printf("Web page sent to client successfully.\n");
        } else {
            char *error =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 18\r\n"
                "Connection: close\r\n\r\n"
                "404 Page Not Found";

            write(client_fd, error, strlen(error));
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
