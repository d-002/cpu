#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif /* !_GNU_SOURCE */

#include "cli.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "queue/queue.h"
#include "utils/errors.h"

#define FLAG_VERBOSE 'v'
#define FLAG_PRINT 'p'
#define FLAG_FILE 'f'

int parse_arg(char c, struct cli_args *out)
{
    char *alloc_ed_optarg;

    switch (c)
    {
    case FLAG_VERBOSE:
        out->verbose = 1;
        break;
    case FLAG_PRINT:
        out->verbose = 1;
        out->run = 0;
        break;
    case FLAG_FILE:
        alloc_ed_optarg = malloc((strlen(optarg) + 1) * sizeof(char));
        memcpy(alloc_ed_optarg, optarg, strlen(optarg) + 1);
        ;
        if (alloc_ed_optarg == NULL)
        {
            log_alloc_error(-1);
            return ALLOC_ERROR;
        }

        return queue_enqueue(out->files_queue, alloc_ed_optarg);
    case '?':
    default:
        logerror(-1, "Error parsing CLI arguments.");
        return CLI_ARGS_ERROR;
    }

    return SUCCESS;
}

int parse_cli_args(int argc, char *argv[], struct cli_args *out)
{
    out->files_queue = queue_create();
    if (out->files_queue == NULL)
    {
        log_alloc_error(-1);
        return ALLOC_ERROR;
    }

    struct option options[] = {
        { "verbose", 0, NULL, FLAG_VERBOSE },
        { "print", 0, NULL, FLAG_PRINT },
        { "file", required_argument, NULL, FLAG_FILE },
        { NULL, 0, NULL, 0 },
    };

    out->run = 1;
    out->verbose = 0;

    char c;
    while (1)
    {
        c = getopt_long(argc, argv, "ha", options, NULL);
        if (c == -1)
            break;

        int res = parse_arg(c, out);
        if (res)
            return res;
    }

    return SUCCESS;
}

void cli_args_destroy(struct cli_args *args)
{
    queue_destroy(args->files_queue);
}
