#include "checks.h"
#include "cli.h"
#include "files.h"

void cleanup(struct cli_args *args)
{
    cli_args_destroy(args);
}

int main(int argc, char *argv[])
{
    int res = checks();
    if (res)
        return res;

    struct cli_args args;
    res = parse_cli_args(argc, argv, &args);

    if (!res)
        res = process_files(&args);

    cleanup(&args);
    return res;
}
