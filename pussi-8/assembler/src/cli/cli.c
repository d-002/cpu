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
#define FLAG_NOEXPORT 'n'
#define FLAG_HELP 'h'

int parse_arg(char c, struct cli_args *out)
{
    switch (c)
    {
    case FLAG_VERBOSE:
        out->verbose = true;
        break;
    case FLAG_PRINT:
        out->print = true;
        break;
    case FLAG_HELP:
        out->help = true;
        break;
    case FLAG_NOEXPORT:
        out->export = false;
        break;
    case '?':
    default:
        logerror(-1, "Error parsing CLI arguments.");
        return CLI_ARGS_ERROR;
    }

    return SUCCESS;
}

int add_file(char *arg, struct cli_args *out)
{
    char *alloc_ed_arg = calloc(strlen(arg) + 1, sizeof(char));
    memcpy(alloc_ed_arg, arg, strlen(arg));

    if (alloc_ed_arg == NULL)
    {
        log_alloc_error(-1);
        return ALLOC_ERROR;
    }

    return queue_enqueue(out->files_queue, alloc_ed_arg);
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
        { "noexport", 0, NULL, FLAG_NOEXPORT },
        { "help", 0, NULL, FLAG_HELP },
        { NULL, 0, NULL, 0 },
    };

    out->export = true;
    out->print = false;
    out->verbose = false;
    out->help = false;

    // handle named arguments
    char c;
    while ((c = getopt_long(argc, argv, "ha", options, NULL)) != -1)
    {
        int res = parse_arg(c, out);
        if (res != SUCCESS)
            return res; // files_queue is freed outside this function
    }

    // handle files
    for (int i = optind; i < argc; i++)
    {
        int res = add_file(argv[i], out);
        if (res != SUCCESS)
            return res;
    }

    return SUCCESS;
}

void cli_args_destroy(struct cli_args *args)
{
    queue_destroy(args->files_queue);
}
