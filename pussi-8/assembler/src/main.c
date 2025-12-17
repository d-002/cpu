#include "cli.h"

void cleanup(struct cli_args *args)
{
    cli_args_destroy(args);
}

int main(int argc, char *argv[])
{
    struct cli_args args;
    int res = parse_cli_args(argc, argv, &args);

    if (res)
        return res;

    cleanup(&args);
    return 0;
}
