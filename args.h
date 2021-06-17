struct args_s {
    unsigned int port;
    void *hostname;
    void *message;
    pthread_mutex_t lock;
};