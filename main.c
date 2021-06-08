#include <stdio.h>
#include <stdlib.h> //atoi
#include <string.h> //strtok
#include "list.h"
#include "server.h"

static bool validate_action(char *action)
{
    if (strncmp("s-talk", action, 6) == 0)
    {
        return true;
    }
    printf("Command \"%s\" not found, expected \"s-talk\"\n", action);
    return false;
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
    char init_conn_command[100] = "s-talk 6060 csil-cpu3 6001";
    const char delim[3] = " \t";
    char action[24];
    unsigned int local_port;
    char server_name[24];
    unsigned int remote_port;

    // scanf("%[^\n]%*c", init_conn_command);
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
    printf("server port: %d\n", remote_port);

    init_server_thread(local_port, "localhost");
    printf("Started server\n");

    // List *MESSAGES = List_create();
    // int num1 = 1;
    // void *p = &num1;
    // List_add(MESSAGES, p);
    // printf("%d\n", List_count(MESSAGES));

    return 0;
}