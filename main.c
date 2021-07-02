#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>  //pthread
#include <stdio.h>
#include <stdlib.h>  //atoi
#include <string.h>  //strtok
#include <sys/socket.h>

#include "args.h"
#include "client.h"
#include "input_reciever.h"
#include "list.h"
#include "message_printer.h"
#include "server.h"
#include "terminate.h"

static const unsigned int DEBUG = 0;

static void validate_argc(int argc) {
    if (argc != 4) {
        printf("Expected 3 arguments, got %d\n",
               argc - 1);  // exclude "./s-talk"
        exit(1);
    }
}

static void validate_port(unsigned int port) {
    if (port > 65353) {
        printf("Port number %d is invalid, maximum value is 65353\n", port);
        exit(1);
    } else if (port < 1023) {
        printf("Port number %d is invalid, ports 0-1023 are usually reserved\n",
               port);
        exit(1);
    }
}

static void validate_getaddrinfo(int res, char *server_name) {
    if (res != 0) {
        printf("Could not find the IPv4 info for the host name \"%s\"\n",
               server_name);
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(res));
        exit(1);
    }
}

// argv[0] is ./s-talk
// argv[1] is local port number
// argv[2] is remote machine name
// argv[3] is remote port number
int main(int argc, char **argv) {
    validate_argc(argc);
    char action[24];
    unsigned int local_port;
    char server_name[24];
    unsigned int remote_port;
    char remote_port_str[24];

    strcpy(action, argv[0]);
    strcpy(server_name, argv[2]);

    local_port = atoi(argv[1]);
    validate_port(local_port);

    remote_port = atoi(argv[3]);
    strcpy(remote_port_str, argv[3]);
    validate_port(remote_port);

    struct addrinfo hints, *getaddrinfo_res;
    char host_ipv4[INET_ADDRSTRLEN];
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // getaddrinfo logic is inspired by:
    // http://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf
    validate_getaddrinfo(
        getaddrinfo(server_name, remote_port_str, &hints, &getaddrinfo_res),
        server_name);
    struct in_addr *addr =
        &(((struct sockaddr_in *)getaddrinfo_res->ai_addr)->sin_addr);
    // getaddrinfo_res->ai_family is AF_INET
    inet_ntop(getaddrinfo_res->ai_family, addr, host_ipv4, sizeof host_ipv4);
    freeaddrinfo(getaddrinfo_res);

    if (DEBUG) {
        printf("action: %s\n", action);
        printf("client port: %d\n", local_port);
        printf("server name: %s\n", server_name);
        printf("server IPv4: %s\n", host_ipv4);
        printf("server port: %d\n\n", remote_port);
    }

    List *messages_to_be_sent = List_create();
    List *messages_to_be_printed = List_create();

    pthread_t await_datagram, get_input, send_datagram, printer;
    pthread_mutex_t messages_to_be_sent_lock, messages_to_be_printed_lock;
    pthread_cond_t unprinted_message_avail, unsent_message_avail;
    pthread_mutex_init(&messages_to_be_sent_lock, NULL);
    pthread_mutex_init(&messages_to_be_printed_lock, NULL);
    pthread_cond_init(&unprinted_message_avail, NULL);
    pthread_cond_init(&unsent_message_avail, NULL);

    struct args_s args_server, args_client, args_input, args_printer;
    args_server.port = local_port;
    args_server.lock = &messages_to_be_printed_lock;
    args_server.list = messages_to_be_printed;
    args_server.cond = &unprinted_message_avail;

    args_client.port = remote_port;
    args_client.list = messages_to_be_sent;
    args_client.lock = &messages_to_be_sent_lock;
    args_client.hostname = host_ipv4;

    args_client.cond = &unsent_message_avail;

    args_input.list = messages_to_be_sent;
    args_input.lock = &messages_to_be_sent_lock;
    args_input.cond = &unsent_message_avail;

    args_printer.list = messages_to_be_printed;
    args_printer.lock = &messages_to_be_printed_lock;
    args_printer.cond = &unprinted_message_avail;

    pthread_create(&await_datagram, NULL, init_server, (void *)&args_server);
    pthread_create(&send_datagram, NULL, init_client, (void *)&args_client);
    pthread_create(&printer, NULL, init_message_printer, (void *)&args_printer);
    pthread_create(&get_input, NULL, init_input_reciever, (void *)&args_input);

    pthread_detach(await_datagram);
    pthread_detach(send_datagram);
    pthread_detach(printer);
    pthread_detach(get_input);

    while (is_running())
        ;

    pthread_cancel(await_datagram);
    pthread_cancel(send_datagram);
    pthread_cancel(printer);
    pthread_cancel(get_input);

    // clear mutexes and cond. variables
    pthread_mutex_destroy(&messages_to_be_sent_lock);
    pthread_mutex_destroy(&messages_to_be_printed_lock);
    pthread_cond_destroy(&unprinted_message_avail);
    pthread_cond_destroy(&unsent_message_avail);

    return 0;
}