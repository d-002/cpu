#include "cli.h"

int main(int argc, char *argv[]) {
    struct cli_args args;
    int res = parse_cli_args(argc, argv, &args);
    if (res)
        return res;

    return 0;
}
