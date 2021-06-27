struct args_s {
    unsigned int port;
    void *hostname;
    void *list;
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
};