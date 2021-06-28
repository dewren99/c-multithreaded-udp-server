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
#include <stdlib.h>      //free
#include <string.h>      //memset
#include <sys/socket.h>  //socket
#include <unistd.h>      //close

#include "args.h"
#include "list.h"

void *init_client(void *_args) {
    // printf("INIT CLIENT\n");
    struct args_s *args = _args;
    unsigned int port = args->port;
    pthread_cond_t *cond = args->cond;
    pthread_mutex_t *lock = args->lock;
    char *hostname = args->hostname;
    List *list = args->list;

    // create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        printf("ERROR: Socket could not be created\n");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(hostname);

    int res;
    while (1) {
        // printf("CLIENT ASK\n");
        pthread_mutex_lock(lock);
        // printf("CLIENT ENTER\n");
        while (!List_count(list)) {
            // printf("CLIENT WAIT\n");
            pthread_cond_wait(cond, lock);
        }
        // printf("CLIENT SEND\n");
        while (List_count(list)) {
            char *message = (char *)List_first(list);
            res = sendto(client_socket, message, strlen(message), 0,
                         (const struct sockaddr *)&server_addr,
                         sizeof server_addr);
            if (res < 0) {
                printf("WARNING: Client could not sent the message \"%s\"\n",
                       message);
            }
            printf("\n CLIENT SENT: %s, strlen: %d, ptr: %p\n", message,
                   strlen(message), message);
            char *ptr_to_be_freed = List_remove(list);
            printf("\nCLIENT TO BE FREED: %p\n", ptr_to_be_freed);
            free(ptr_to_be_freed);
            printf("\nCLIENT FREED: %p\n", ptr_to_be_freed);
        }
        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    close(client_socket);
    return NULL;
}