#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const char TERMINATE[] = {'!'};
static bool RUN = true;

static void set_running(bool should_run) { RUN = should_run; }

void should_program_terminate(char *message) {
    if (strncmp(TERMINATE, message, sizeof message) == 0) {
        set_running(false);
        // exit(0);
    }
}

bool is_running() { return RUN; }