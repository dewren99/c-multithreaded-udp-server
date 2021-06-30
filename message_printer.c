#include <pthread.h>  //pthread
#include <stdio.h>
#include <stdlib.h>  //free
#include <string.h>

#include "args.h"
#include "list.h"
#include "terminate.h"

void *init_message_printer(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t *lock = args->lock;
    pthread_cond_t *cond = args->cond;
    List *list = args->list;
    while (1) {
        pthread_mutex_lock(lock);
        while (!List_count(list)) {
            pthread_cond_wait(cond, lock);
        }
        while (List_count(list)) {
            char *message = (char *)List_first(list);
            printf("< %s\n", message);
            should_program_terminate(message);
            free(List_remove(list));
        }
        pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
    }

    return NULL;
}