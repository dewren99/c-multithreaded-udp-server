#include <pthread.h>  //pthread
#include <stdio.h>

#include "args.h"
#include "list.h"

void *init_message_printer(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t lock = args->lock;
    List *list = args->message;

    while (1) {
        pthread_mutex_lock(&lock);
        if (List_count(list)) {
            printf("printer: %s %d\n", (char *)List_first(list),
                   List_count(list));
            List_next(list);
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}