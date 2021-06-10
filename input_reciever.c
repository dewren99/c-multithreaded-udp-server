#include <stdio.h>
#include <stdbool.h>
#include <string.h> //strtok

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

void *init_input_reciever(void *input_buffer)
{
    printf("[INPUT RECIEVER] \n");
    while (1)
    {
        printf("Enter your message: ");
        scanf("%[^\n]%*c", (char *)input_buffer);
        printf("Entered: %s\n", (char *)input_buffer);
        return input_buffer;
    }
}