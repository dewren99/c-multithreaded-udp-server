#include <pthread.h>  //pthread
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  //exit
#include <string.h>  //strtok

#include "args.h"
#include "list.h"

static const char TERMINATE[] = {'!'};
static const unsigned int MAX_MESSAGE_LEN = 1024;
static const unsigned int MAX_MESSAGE_SIZE = sizeof(char) * MAX_MESSAGE_LEN;

void *init_input_reciever(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t lock = args->lock;
    List *list = args->message;

    while (1) {
        char *message_slot = (char *)calloc(MAX_MESSAGE_LEN, sizeof(char));
        pthread_mutex_lock(&lock);
        printf("> ");
        // scanf("%[^\n]%*c", message_slot);
        fgets(message_slot, MAX_MESSAGE_SIZE, stdin);
        message_slot[strlen(message_slot) - 1] =
            '\0';  // remove trailing whitespace

        if (strncmp(TERMINATE, message_slot, sizeof TERMINATE) == 0) {
            exit(0);
        }

        if (strlen(message_slot)) {
            List_add(list, (void *)message_slot);
        }

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}