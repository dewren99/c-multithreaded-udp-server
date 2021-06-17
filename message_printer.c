#include <pthread.h>  //pthread
#include <stdio.h>
#include <string.h>

#include "args.h"
#include "list.h"

void *init_message_printer(void *_args) {
    struct args_s *args = _args;
    pthread_mutex_t lock = args->lock;
    List *list = args->message;
    char *message;
    while (1) {
        pthread_mutex_lock(&lock);
        while (List_count(list)) {
            message = (char *)List_first(list);
            printf("printer: %s\n", message);
            List_remove(list);
        }
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}