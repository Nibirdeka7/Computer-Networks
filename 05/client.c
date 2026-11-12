#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd, n;
    unsigned long long fact;
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    printf("Enter a number: ");
    scanf("%d", &n);

    sendto(sockfd, &n, sizeof(n), 0,
           (struct sockaddr *)&server, len);

    recvfrom(sockfd, &fact, sizeof(fact), 0,
             (struct sockaddr *)&server, &len);

    printf("Factorial of %d = %llu\n", n, fact);

    close(sockfd);
    return 0;
}