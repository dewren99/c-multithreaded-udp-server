#include <pthread.h>  //pthread
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  //exit
#include <string.h>  //strtok

#include "args.h"
#include "list.h"

static const char TERMINATE[] = {'!'};

static char message_slots[LIST_MAX_NUM_NODES][1024];
static unsigned int slot_i = 0;
static void inc_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i + 1) % LIST_MAX_NUM_NODES;
}
static void dec_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i - 1) % LIST_MAX_NUM_NODES;
}

void *init_input_reciever(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t lock = args->lock;
    List *list = args->message;

    while (1) {
        pthread_mutex_lock(&lock);
        memset(&message_slots[slot_i], 0, sizeof message_slots[slot_i]);
        printf("> ");
        scanf("%[^\n]%*c", message_slots[slot_i]);
        List_add(list, &message_slots[slot_i]);

        if (strncmp(TERMINATE, message_slots[slot_i], sizeof TERMINATE) == 0) {
            exit(0);
        }

        inc_slot();
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}