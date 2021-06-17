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

static const char TERMINATE[] = {'!'};

static char message_slots[LIST_MAX_NUM_NODES][1024];
static unsigned int slot_i = 0;
static void inc_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i + 1) % LIST_MAX_NUM_NODES;
}
static void dec_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i - 1) % LIST_MAX_NUM_NODES;
}

void *init_server(void *_args) {
    struct args_s *args = _args;
    unsigned int port = args->port;
    List *list = args->message;
    // char *hostname = args->hostname;
    pthread_mutex_t lock = args->lock;
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
        // return NULL;
    }

    while (1) {
        pthread_mutex_lock(&lock);
        memset(&message_slots[slot_i], 0, sizeof message_slots[slot_i]);
        res = recvfrom(server_socket, message_slots[slot_i],
                       sizeof message_slots[slot_i], 0,
                       (struct sockaddr *)&client_addr, &client_addr_len);
        if (res < 0) {
            printf("[SERVER] Could not recieve incoming messages\n");
            return NULL;
        }

        const char *client_ipv4 = inet_ntoa(client_addr.sin_addr);
        const uint16_t client_port = ntohs(client_addr.sin_port);
        // printf("[SERVER] received message from address: %s:%d\n",
        // client_ipv4, client_port);
        // printf("server: %s\n", message_slots[slot_i]);

        if (strncmp(TERMINATE, message_slots[slot_i], sizeof TERMINATE) == 0) {
            pthread_mutex_unlock(&lock);
            break;
        }

        List_add(list, (void *)&message_slots[slot_i]);
        inc_slot();

        pthread_mutex_unlock(&lock);
    }

    close(server_socket);
    return NULL;
}