#include <pthread.h>  //pthread
#include <stdio.h>
#include <stdlib.h>  //free
#include <string.h>

#include "args.h"
#include "list.h"

void *init_message_printer(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t *lock = args->lock;
    pthread_cond_t *cond = args->cond;
    List *list = args->list;
    while (1) {
        pthread_mutex_lock(lock);
        while (!List_count(list)) {
            // printf("MESSAGE PRINTER WAITING\n");
            pthread_cond_wait(cond, lock);
        }
        while (List_count(list)) {
            char *message = (char *)List_first(list);

            printf("\nPRINTER GOT: %s, strlen: %d, ptr: %p\n", message,
                   strlen(message), message);
            char *ptr_to_be_freed = List_remove(list);
            printf("\nPRINTER TO BE FREED: %p\n", ptr_to_be_freed);
            free(ptr_to_be_freed);
            printf("\nPRINTER FREED: %p\n", ptr_to_be_freed);
        }
        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    return NULL;
}