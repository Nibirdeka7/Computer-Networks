#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    int n;
    unsigned long long fact;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("UDP Factorial Server started...\n");

    while (1) {
        recvfrom(sockfd, &n, sizeof(n), 0,
                 (struct sockaddr *)&client, &len);

        fact = 1;
        for (int i = 1; i <= n; i++)
            fact *= i;

        sendto(sockfd, &fact, sizeof(fact), 0,
               (struct sockaddr *)&client, len);

        printf("Factorial of %d = %llu\n", n, fact);
    }

    close(sockfd);
    return 0;
}