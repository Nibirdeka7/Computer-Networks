#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9090

/*
 * Receive exactly 'size' bytes.
 */
int recv_all(int sock, void *buffer, int size) {

    int total = 0;

    char *ptr = (char *)buffer;

    while (total < size) {

        int n = recv(sock,
                     ptr + total,
                     size - total,
                     0);

        if (n <= 0)
            return -1;

        total += n;
    }

    return total;
}

/*
 * Send exactly 'size' bytes.
 */
int send_all(int sock, void *buffer, int size) {

    int total = 0;

    char *ptr = (char *)buffer;

    while (total < size) {

        int n = send(sock,
                     ptr + total,
                     size - total,
                     0);

        if (n <= 0)
            return -1;

        total += n;
    }

    return total;
}

int main() {

    int sock;

    struct sockaddr_in server;

    /*
     * Create socket.
     */
    sock = socket(AF_INET,
                  SOCK_STREAM,
                  0);

    if (sock < 0) {

        perror("Socket creation failed");
        return 1;
    }

    /*
     * Server configuration.
     */
    server.sin_family = AF_INET;

    server.sin_port = htons(PORT);

    server.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    /*
     * Connect to server.
     */
    if (connect(sock,
                (struct sockaddr *)&server,
                sizeof(server)) < 0) {

        perror("Connection failed");

        close(sock);

        return 1;
    }

    printf("====================================\n");
    printf("       GO-BACK-N RECEIVER\n");
    printf("====================================\n");
    printf("Connected to server.\n\n");

    /*
     * Receiver expects Packet 0 initially.
     */
    int expected = 0;

    while (1) {

        int packet;

        /*
         * Receive packet.
         */
        int n = recv_all(sock,
                         &packet,
                         sizeof(packet));

        if (n <= 0) {

            printf("Server disconnected.\n");
            break;
        }

        /*
         * -1 means transmission is complete.
         */
        if (packet == -1) {

            printf("\n====================================\n");
            printf(" Transmission completed successfully\n");
            printf("====================================\n");

            break;
        }

        printf("Received Packet %d\n", packet);

        /*
         * Correct packet received.
         */
        if (packet == expected) {

            printf("Packet %d ACCEPTED\n",
                   packet);

            /*
             * Send ACK for this packet.
             */
            int ack = packet;

            if (send_all(sock,
                         &ack,
                         sizeof(ack)) < 0) {

                printf("Failed to send ACK.\n");
                break;
            }

            printf("Sent ACK %d\n\n",
                   ack);

            /*
             * Now expect next packet.
             */
            expected++;
        }

        /*
         * Out-of-order packet.
         */
        else {

            printf("Packet %d OUT OF ORDER\n",
                   packet);

            printf("Expected Packet %d\n",
                   expected);

            printf("Packet %d DISCARDED\n",
                   packet);

            /*
             * Send duplicate ACK
             * for previous correctly received packet.
             */
            int ack = expected - 1;

            if (send_all(sock,
                         &ack,
                         sizeof(ack)) < 0) {

                printf("Failed to send duplicate ACK.\n");
                break;
            }

            printf("Sent Duplicate ACK %d\n\n",
                   ack);
        }
    }

    close(sock);

    return 0;
}