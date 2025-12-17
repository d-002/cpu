#define _GNU_SOURCE

#include "cli.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "logger.h"
#include "queue.h"

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
        out->run = 0;
        break;
    case FLAG_PRINT:
        out->run = 0;
        break;
    case FLAG_FILE:
        if (optarg == NULL)
        {
            logerror("Missing argument for -f.");
            return CLI_ARGS_ERROR;
        }
        alloc_ed_optarg = malloc((strlen(optarg) + 1) * sizeof(char));
        ;
        if (alloc_ed_optarg == NULL)
        {
            log_alloc_error();
            return ALLOC_ERROR;
        }

        return queue_enqueue(out->files_queue, alloc_ed_optarg);
    }

    return SUCCESS;
}

int parse_cli_args(int argc, char *argv[], struct cli_args *out)
{
    out->files_queue = queue_create();
    if (out->files_queue == NULL)
    {
        log_alloc_error();
        return ALLOC_ERROR;
    }

    struct option options[] = {
        { "verbose", 0, NULL, FLAG_VERBOSE },
        { "print", 0, NULL, FLAG_PRINT },
        { "file", required_argument, NULL, FLAG_FILE },
        { NULL, 0, NULL, 0 },
    };

    char c;
    while (1)
    {
        c = getopt_long(argc, argv, "ha", options, NULL);
        if (c == -1)
            break;

        int res = parse_arg(c, out);
        if (res)
        {
            queue_destroy(out->files_queue);
            return res;
        }
    }

    out->run = 1;
    out->verbose = 0;
    return SUCCESS;
}
