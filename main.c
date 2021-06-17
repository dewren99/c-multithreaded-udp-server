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

static bool validate_action(char *action) {
    return strncmp("s-talk", action, 6) == 0
               ? true
               : printf("Command \"%s\" not found, expected \"s-talk\"\n",
                        action) &&
                     false;
}

static bool validate_port(unsigned int port) {
    if (port > 65353) {
        printf("Port number %d is invalid, maximum value is 65353\n", port);
        return false;
    } else if (port < 1023) {
        printf("Port number %d is invalid, ports 0-1023 are usually reserved\n",
               port);
        return false;
    }
    return true;
}

int main() {
    printf("[S-TALK] \n");
    // char init_conn_command[100] = "s-talk 6060 csil-cpu3 6001";
    char init_conn_command[100];
    const char delim[3] = " \t";
    char action[24];
    unsigned int local_port;
    char server_name[24];
    unsigned int remote_port;

    printf("Initiate the simple talk: ");
    scanf("%[^\n]%*c", init_conn_command);
    char *token = strtok(init_conn_command, delim);

    strcpy(action, token);
    if (!validate_action(action)) {
        return -1;
    }
    token = strtok(NULL, delim);
    local_port = atoi(token);
    if (!validate_port(local_port)) {
        return -1;
    }
    token = strtok(NULL, delim);
    strcpy(server_name, token);
    token = strtok(NULL, delim);
    remote_port = atoi(token);
    if (!validate_port(remote_port)) {
        return -1;
    }

    struct hostent *hostbyname = gethostbyname(server_name);
    if (!hostbyname) {
        printf("Could not find the host by the name %s\n", server_name);
        return -1;
    }

    printf("action: %s\n", action);
    printf("client port: %d\n", local_port);
    printf("server name: %s\n", server_name);
    printf("server port: %d\n\n", remote_port);

    List *messages_to_be_sent = List_create();
    List *messages_to_be_printed = List_create();

    pthread_t await_datagram, get_input, send_datagram, printer;
    pthread_mutex_t messages_to_be_sent_lock, messages_to_be_printed_lock;
    pthread_mutex_init(&messages_to_be_sent_lock, NULL);
    pthread_mutex_init(&messages_to_be_printed_lock, NULL);

    struct args_s args_server, args_client, args_input, args_printer;
    args_server.port = local_port;
    args_server.lock = messages_to_be_printed_lock;
    args_server.message = messages_to_be_printed;

    args_client.port = remote_port;
    args_client.message = messages_to_be_sent;
    args_client.lock = messages_to_be_sent_lock;
    args_client.hostname =
        inet_ntoa(*((struct in_addr *)hostbyname->h_addr_list[0]));

    args_input.message = messages_to_be_sent;
    args_input.lock = messages_to_be_sent_lock;

    args_printer.message = messages_to_be_printed;
    args_printer.lock = messages_to_be_printed_lock;

    pthread_create(&await_datagram, NULL, init_server, (void *)&args_server);
    pthread_create(&get_input, NULL, init_input_reciever, (void *)&args_input);
    pthread_create(&send_datagram, NULL, init_client, (void *)&args_client);
    pthread_create(&printer, NULL, init_message_printer, (void *)&args_printer);

    while (1)
        ;

    pthread_mutex_destroy(&messages_to_be_sent_lock);
    pthread_mutex_destroy(&messages_to_be_printed_lock);

    return 0;
}