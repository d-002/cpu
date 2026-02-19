#include <stdio.h>

#include "cli/cli.h"
#include "files/files.h"
#include "utils/checks.h"
#include "utils/errors.h"

int show_help()
{
    printf("Temporary help message, see repo readme instead.\n");

    return SUCCESS;
}

int main(int argc, char *argv[])
{
    int res = checks();
    if (res != SUCCESS)
        return res;

    struct cli_args args;
    res = parse_cli_args(argc, argv, &args);

    if (res == SUCCESS)
        res = args.help ? show_help() : process_files(&args);

    cli_args_destroy(&args);
    return res;
}
