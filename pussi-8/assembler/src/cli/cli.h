#ifndef CLI_H
#define CLI_H

#include "queue/queue.h"

struct cli_args
{
    int run;
    int verbose;
    struct queue *files_queue;
};

int parse_cli_args(int argc, char *argv[], struct cli_args *out);
void cli_args_destroy(struct cli_args *args);

#endif /* ! CLI_H */
