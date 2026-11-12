#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int sock;

void *receiveMessages(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes = read(sock, buffer, BUFFER_SIZE);

        if (bytes <= 0) {
            printf("\nDisconnected from server.\n");
            exit(0);
        }

        printf("\nFriend: %s", buffer);
        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    pthread_t tid;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    printf("Connected to chat server.\n");

    pthread_create(&tid, NULL, receiveMessages, NULL);

    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);

    return 0;
}