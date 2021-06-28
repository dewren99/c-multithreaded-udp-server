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
    pthread_mutex_t *lock = args->lock;
    pthread_cond_t *cond = args->cond;
    List *list = args->list;

    while (1) {
        // printf("INPUT RECIEVER ASK\n");
        char *message_slot = (char *)calloc(MAX_MESSAGE_LEN, sizeof(char));
        pthread_mutex_lock(lock);
        while (List_count(list)) {
            // printf("INPUT RECIEVER WAIT - there are unsent messages\n");
            pthread_cond_wait(cond, lock);
        }
        // printf("INPUT RECIEVER PASS\n");
        // printf("> ");
        // scanf("%[^\n]%*c", message_slot);
        fgets(message_slot, MAX_MESSAGE_SIZE, stdin);
        message_slot[strlen(message_slot) - 1] =
            '\0';  // remove trailing whitespace

        if (strncmp(TERMINATE, message_slot, sizeof TERMINATE) == 0) {
            free(message_slot);
            exit(0);
        }

        if (strlen(message_slot)) {
            if (List_add(list, (void *)message_slot) == -1) {
                printf(
                    "\nERROR: Keyboard listener could not process the entered "
                    "message \"%s\"\n",
                    message_slot);
                free(message_slot);
            }
        } else {
            free(message_slot);
        }

        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    return NULL;
}