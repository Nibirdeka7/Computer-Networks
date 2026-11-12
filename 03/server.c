#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define PORT 9090
#define TOTAL_PACKETS 10
#define WINDOW_SIZE 4
#define TIMEOUT 3
#define LOSS_PROBABILITY 30

/* Send exactly 'size' bytes */
int send_all(int sock, void *buffer, int size) {
    int total = 0;
    char *ptr = (char *)buffer;

    while (total < size) {
        int n = send(sock, ptr + total, size - total, 0);

        if (n <= 0)
            return -1;

        total += n;
    }

    return total;
}

/* Receive exactly 'size' bytes */
int recv_all(int sock, void *buffer, int size) {
    int total = 0;
    char *ptr = (char *)buffer;

    while (total < size) {
        int n = recv(sock, ptr + total, size - total, 0);

        if (n <= 0)
            return -1;

        total += n;
    }

    return total;
}

/* Simulate packet loss */
int packet_lost() {
    int random_number = rand() % 100;

    return random_number < LOSS_PROBABILITY;
}

int main() {

    int server_fd, client_fd;

    struct sockaddr_in server, client;

    socklen_t len = sizeof(client);

    srand(time(NULL));

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    /* Allow reuse of port */
    int opt = 1;

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    /* Server configuration */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    /* Bind */
    if (bind(server_fd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0) {

        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    /* Listen */
    if (listen(server_fd, 1) < 0) {

        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("====================================\n");
    printf("      GO-BACK-N SENDER SERVER\n");
    printf("====================================\n");
    printf("Total Packets : %d\n", TOTAL_PACKETS);
    printf("Window Size   : %d\n", WINDOW_SIZE);
    printf("Timeout       : %d seconds\n", TIMEOUT);
    printf("Loss Chance   : %d%%\n", LOSS_PROBABILITY);
    printf("====================================\n\n");

    printf("Waiting for client...\n");

    /* Accept client */
    client_fd = accept(server_fd,
                       (struct sockaddr *)&client,
                       &len);

    if (client_fd < 0) {

        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    printf("Client connected.\n\n");

    int base = 0;
    int next_seq = 0;

    while (base < TOTAL_PACKETS) {

        /*
         * Send packets within current window.
         */
        while (next_seq < base + WINDOW_SIZE &&
               next_seq < TOTAL_PACKETS) {

            printf("Sending Packet %d", next_seq);

            /*
             * Simulate packet loss.
             */
            if (packet_lost()) {

                printf("  ---> LOST!\n");

                /*
                 * Packet is NOT sent.
                 * Receiver will never receive it.
                 */
            }
            else {

                if (send_all(client_fd,
                             &next_seq,
                             sizeof(next_seq)) < 0) {

                    printf("Connection lost.\n");
                    close(client_fd);
                    close(server_fd);
                    return 1;
                }

                printf("  ---> Sent\n");
            }

            next_seq++;
        }

        /*
         * Wait for ACK.
         */
        fd_set readfds;

        struct timeval tv;

        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);

        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        int result = select(client_fd + 1,
                            &readfds,
                            NULL,
                            NULL,
                            &tv);

        /*
         * TIMEOUT
         */
        if (result == 0) {

            printf("\n*** TIMEOUT ***\n");

            printf("No ACK received for Packet %d\n", base);

            printf("Go-Back-N: Retransmitting from Packet %d\n\n",
                   base);

            /*
             * Go back to oldest unacknowledged packet.
             */
            next_seq = base;

        }

        /*
         * ACK received
         */
        else if (result > 0) {

            int ack;

            if (recv_all(client_fd,
                         &ack,
                         sizeof(ack)) < 0) {

                printf("Client disconnected.\n");
                break;
            }

            printf("Received ACK %d\n", ack);

            /*
             * ACK represents the highest
             * consecutively received packet.
             */
            if (ack >= base) {

                base = ack + 1;

                printf("Window moved. New base = %d\n\n",
                       base);
            }
        }

        else {

            perror("select failed");
            break;
        }
    }

    /*
     * Transmission complete.
     */
    int end = -1;

    send_all(client_fd, &end, sizeof(end));

    printf("====================================\n");
    printf(" All packets transmitted successfully\n");
    printf("====================================\n");

    close(client_fd);
    close(server_fd);

    return 0;
}