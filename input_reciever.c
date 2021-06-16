#include <pthread.h>  //pthread
#include <stdbool.h>
#include <stdio.h>
#include <string.h>  //strtok

#include "args.h"
#include "list.h"

static char message_slots[LIST_MAX_NUM_NODES][1024];
static unsigned int slot_i = 0;
static void inc_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i + 1) % LIST_MAX_NUM_NODES;
}
static void dec_slot() {
    slot_i = (LIST_MAX_NUM_NODES + slot_i - 1) % LIST_MAX_NUM_NODES;
}

static void print_str_list(List *list) {
    // printf("[head -> tail]: ");
    // Node *p = list->head;
    // while (p) {
    //     void *item = p->item;
    //     item ? printf("%s", (char *)item) : printf("%p", item);
    //     p = p->nextNode;
    //     if (p) {
    //         printf(" -> ");
    //     }
    // }
    // printf("\n");
}

void *init_input_reciever(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t lock = args->lock;
    List *list = args->message;

    while (1) {
        // memset((void *)input_buffer, 0, sizeof input_buffer);
        pthread_mutex_lock(&lock);
        printf("> ");
        scanf("%[^\n]%*c", message_slots[slot_i]);
        List_add(list, &message_slots[slot_i]);
        inc_slot();
        // printf("Me: %s\n", (char *)List_curr(list));

        if (strcmp((char *)List_curr(list), "print") == 0) {
            printf("PRINT\n");
            print_str_list(list);
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}