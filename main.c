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

static void validate_hostbyname(struct hostent *hostbyname, char *server_name) {
    if (!hostbyname) {
        printf("Could not find the host by the name %s\n", server_name);
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

    strcpy(action, argv[0]);
    strcpy(server_name, argv[2]);

    local_port = atoi(argv[1]);
    validate_port(local_port);

    remote_port = atoi(argv[3]);
    validate_port(remote_port);

    struct hostent *hostbyname = gethostbyname(server_name);
    validate_hostbyname(hostbyname, server_name);

    if (DEBUG) {
        printf("action: %s\n", action);
        printf("client port: %d\n", local_port);
        printf("server name: %s\n", server_name);
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
    args_client.hostname =
        inet_ntoa(*((struct in_addr *)hostbyname->h_addr_list[0]));
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

    while (1)
        ;

    // clear mutexes and cond. variables
    pthread_mutex_destroy(&messages_to_be_sent_lock);
    pthread_mutex_destroy(&messages_to_be_printed_lock);
    pthread_cond_destroy(&unprinted_message_avail);
    pthread_cond_destroy(&unsent_message_avail);

    return 0;
}