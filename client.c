// Insprations:
// https://www.educative.io/edpresso/how-to-implement-udp-sockets-in-c
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// http://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf Chapter 6
// https://coursys.sfu.ca/2021su-cmpt-300-d1/pages/tut_sockets/view
#include "client.h"

#include <arpa/inet.h>  //inet_ntoa
#include <netinet/in.h>
#include <pthread.h>  //pthread
#include <stdio.h>
#include <string.h>      //memset
#include <sys/socket.h>  //socket
#include <unistd.h>      //close

#include "args.h"
#include "list.h"

void *init_client(void *_args) {
    // printf("[CLIENT] \n");

    struct args_s *args = _args;
    unsigned int port = args->port;
    List *list = args->message;
    char *message;
    pthread_mutex_t lock = args->lock;
    int res;
    // const char client_msg[] = "HELLO";

    // create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        printf("Socket could not be created\n");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;  // use the IP of the local
                                               // machine

    while (1) {
        pthread_mutex_lock(&lock);
        if (list && List_count(list)) {
            message = (char *)List_first(list);
            printf("CLIENT START %s\n", (char *)List_curr(list));
            res = sendto(client_socket, message, strlen(message), 0,
                         (const struct sockaddr *)&server_addr,
                         sizeof server_addr);
            if (res < 0) {
                printf("Message could not be sent\n");
            }
            printf("CLIENT HERE\n");
            List_remove(list);
            printf("CLIENT END\n");
        }
        pthread_mutex_unlock(&lock);
    }

    close(client_socket);
    return NULL;
}