#include <pthread.h>  //pthread
#include <stdio.h>
#include <stdlib.h>  //atoi
#include <string.h>  //strtok

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

    printf("action: %s\n", action);
    printf("client port: %d\n", local_port);
    printf("server name: %s\n", server_name);
    printf("server port: %d\n\n", remote_port);

    List *list = List_create();
    pthread_t await_datagram, get_input, send_datagram;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    struct args_s args_server, args_client, args_input, args_printer;
    args_server.port = local_port;
    args_server.hostname = server_name;
    args_server.lock = lock;
    args_server.message = list;

    args_client.port = remote_port;
    args_client.lock = lock;
    args_client.message = list;

    args_input.message = list;
    args_input.lock = lock;
    args_printer.message = list;
    args_printer.lock = lock;

    pthread_create(&await_datagram, NULL, init_server, (void *)&args_server);
    pthread_create(&get_input, NULL, init_input_reciever, (void *)&args_input);
    pthread_create(&send_datagram, NULL, init_client, (void *)&args_client);
    pthread_create(&send_datagram, NULL, init_message_printer,
                   (void *)&args_client);

    while (1)
        ;

    return 0;
}