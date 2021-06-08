// Insprations:
// https://www.educative.io/edpresso/how-to-implement-udp-sockets-in-c
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// http://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf Chapter 6
// https://coursys.sfu.ca/2021su-cmpt-300-d1/pages/tut_sockets/view
#include <stdio.h>
#include <sys/socket.h> //socket
#include <netinet/in.h>
#include <arpa/inet.h> //inet_ntoa
#include <string.h>    //memset
#include <unistd.h>    //close
#include <pthread.h>
#include "server.h"

// #define PORT 8080

struct args_s
{
    unsigned int port;
    char *hostname;
};

static void init_server(void *arguments)
{
    printf("[SERVER] \n");
    printf("Student Name: Deniz Evrendilek \n");
    printf("Student ID: 301340591 \n\n");

    struct args_s *args = arguments;
    unsigned int port = args->port;
    char *hostname = args->hostname;
    char client_msg[1024];
    struct sockaddr_in client_addr, server_addr;
    int client_addr_len = sizeof client_addr;
    memset(client_msg, '\0', sizeof client_msg);

    //create socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
    {
        printf("[SERVER] Could not create the socket\n");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY; //use the IP of the local machine

    int res = bind(server_socket, (struct sockaddr *)&server_addr, sizeof server_addr);
    if (res < 0)
    {
        printf("[SERVER] Port could not be binded\n");
        return;
    }

    while (1)
    {
        printf("[SERVER] waiting for connections...\n\n");

        //busy wait
        res = recvfrom(server_socket, client_msg, sizeof client_msg, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (res < 0)
        {
            printf("[SERVER] Could not recieve incoming messages\n");
            return;
        }

        const char *client_ipv4 = inet_ntoa(client_addr.sin_addr);
        const uint16_t client_port = ntohs(client_addr.sin_port);
        printf("[SERVER] received message from address: %s:%d\n", client_ipv4, client_port);
        printf("[SERVER] message recieved: %s\n", client_msg);

        char exit[] = {'!'};
        if (strncmp(exit, client_msg, sizeof exit) == 0)
        {
            break;
        }
    }

    close(server_socket);
    return;
}

void init_server_thread(unsigned int port, char *hostname)
{
    pthread_t await_datagram;
    struct args_s args;
    args.port = port;
    args.hostname = hostname;
    pthread_create(&await_datagram, NULL, (void *)init_server, (void *)&args);
    pthread_join(await_datagram, NULL);
}