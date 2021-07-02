#include <stdlib.h>
#include <string.h>

static const char TERMINATE[] = {'!'};

void should_program_terminate(char *message) {
    if (strncmp(TERMINATE, message, sizeof message) == 0) {
        exit(0);
    }
}