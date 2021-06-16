struct message_s {
    unsigned int sender;  // 0 if local, > 1 if remote
    char message[1024];
};