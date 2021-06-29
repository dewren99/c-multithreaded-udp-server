#include <pthread.h>  //pthread
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  //exit
#include <string.h>  //strtok

#include "args.h"
#include "list.h"

static const char TERMINATE[] = {'!'};
static const unsigned int MAX_MESSAGE_LEN = 100;

void *init_input_reciever(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t *lock = args->lock;
    pthread_cond_t *cond = args->cond;
    List *list = args->list;

    while (1) {
        pthread_mutex_lock(lock);
        while (List_count(list)) {
            pthread_cond_wait(cond, lock);
        }
        char *message_slot = calloc(MAX_MESSAGE_LEN, sizeof *message_slot);
        fgets(message_slot, MAX_MESSAGE_LEN, stdin);
        message_slot[strlen(message_slot) - 1] =
            '\0';  // remove trailing whitespace

        if (strncmp(TERMINATE, message_slot, sizeof TERMINATE) == 0) {
            free(message_slot);
            exit(0);
        }

        if (message_slot && strlen(message_slot)) {
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