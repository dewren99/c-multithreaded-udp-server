// Insprations:
// https://www.educative.io/edpresso/how-to-implement-udp-sockets-in-c
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// http://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf Chapter 6
// https://coursys.sfu.ca/2021su-cmpt-300-d1/pages/tut_sockets/view

#include "server.h"

#include <arpa/inet.h>  //inet_ntoa
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>  //pthread
#include <stdio.h>
#include <stdlib.h>
#include <string.h>      //memset
#include <sys/socket.h>  //socket
#include <unistd.h>      //close

#include "args.h"
#include "list.h"

static const unsigned int MAX_INCOMING_MESSAGE_LEN = 4096;

void *init_server(void *_args) {
    struct args_s *args = (struct args_s *)_args;
    unsigned int port = args->port;
    List *list = args->list;
    pthread_cond_t *cond = args->cond;
    pthread_mutex_t *lock = args->lock;
    struct sockaddr_in client_addr, server_addr;
    int client_addr_len = sizeof client_addr;

    // create socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        printf("[SERVER] Could not create the socket\n");
        return NULL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(server_socket, (struct sockaddr *)&server_addr,
                   sizeof server_addr);
    if (res < 0) {
        printf("[SERVER] Port could not be binded\n");
        exit(1);
    }

    while (1) {
        pthread_mutex_lock(lock);
        while (List_count(list)) {
            pthread_cond_wait(cond, lock);
        }
        char *message_slot =
            calloc(MAX_INCOMING_MESSAGE_LEN, sizeof *message_slot);
        res = recvfrom(server_socket, message_slot, MAX_INCOMING_MESSAGE_LEN, 0,
                       (struct sockaddr *)&client_addr, &client_addr_len);
        if (res < 0) {
            printf("[SERVER] Could not recieve incoming messages\n");
            exit(1);
        }

        // const char *client_ipv4 = inet_ntoa(client_addr.sin_addr);
        // const uint16_t client_port = ntohs(client_addr.sin_port);

        if (message_slot && strlen(message_slot)) {
            if (List_add(list, (void *)message_slot) == -1) {
                printf(
                    "\nWARNING: Server could not add \"%s\" to the messages to "
                    "be "
                    "printed list\n",
                    message_slot);
                free(message_slot);
            }
        } else {
            free(message_slot);
        }
        message_slot = NULL;
        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    close(server_socket);
    return NULL;
}