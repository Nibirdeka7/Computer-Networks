#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {

    int sockfd;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    struct timeval start, end;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    gettimeofday(&start, NULL);

    sendto(
        sockfd,
        buffer,
        strlen(buffer),
        0,
        (struct sockaddr *)&server_addr,
        server_len
    );

    int n = recvfrom(
        sockfd,
        buffer,
        BUFFER_SIZE - 1,
        0,
        (struct sockaddr *)&server_addr,
        &server_len
    );

    if (n < 0) {
        perror("recvfrom failed");
        close(sockfd);
        exit(1);
    }

    gettimeofday(&end, NULL);

    buffer[n] = '\0';

    double rtt =
        (end.tv_sec - start.tv_sec) * 1000.0 +
        (end.tv_usec - start.tv_usec) / 1000.0;

    printf("Server response: %s\n", buffer);
    printf("Round Trip Time (RTT): %.3f ms\n", rtt);

    close(sockfd);

    return 0;
}