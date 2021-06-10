#include <stdio.h>
#include <stdlib.h>  //atoi
#include <string.h>  //strtok
#include <pthread.h> //pthread
#include "list.h"
#include "server.h"
#include "client.h"
#include "args.h"
#include "input_reciever.h"

static bool validate_action(char *action)
{
    return strncmp("s-talk", action, 6) == 0 ? true : printf("Command \"%s\" not found, expected \"s-talk\"\n", action) && false;
}

static bool validate_port(unsigned int port)
{
    if (port > 65353)
    {
        printf("Port number %d is invalid, maximum value is 65353\n", port);
        return false;
    }
    else if (port < 1023)
    {
        printf("Port number %d is invalid, ports 0-1023 are usually reserved\n", port);
        return false;
    }
    return true;
}

int main()
{
    printf("[MAIN] \n");
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
    if (!validate_action(action))
    {
        return -1;
    }
    token = strtok(NULL, delim);
    local_port = atoi(token);
    if (!validate_port(local_port))
    {
        return -1;
    }
    token = strtok(NULL, delim);
    strcpy(server_name, token);
    token = strtok(NULL, delim);
    remote_port = atoi(token);
    if (!validate_port(remote_port))
    {
        return -1;
    }

    printf("action: %s\n", action);
    printf("client port: %d\n", local_port);
    printf("server name: %s\n", server_name);
    printf("server port: %d\n\n", remote_port);

    pthread_t await_datagram, get_input, send_datagram;
    struct args_s args_server, args_client;
    args_server.port = local_port;
    args_server.hostname = server_name;
    args_client.port = remote_port;
    char input_buffer[100];

    pthread_create(&await_datagram, NULL, init_server, (void *)&args_server);
    printf("Started server\n");
    pthread_create(&get_input, NULL, init_input_reciever, (void *)input_buffer);
    printf("Started input reciever\n");
    pthread_join(get_input, NULL); // cannot send datagram without input
    args_client.message = input_buffer;
    pthread_create(&send_datagram, NULL, init_client, (void *)&args_client);
    printf("Started client\n");

    // pthread_join(await_datagram, NULL);
    pthread_join(send_datagram, NULL);

    return 0;
}