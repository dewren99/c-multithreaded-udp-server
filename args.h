struct args_s {
    unsigned int port;
    char *hostname;
    void *message;
    pthread_mutex_t lock;
};