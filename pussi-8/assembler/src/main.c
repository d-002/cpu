#include "cli.h"
#include "queue.h"

void cleanup(struct cli_args *args) {
    queue_destroy(args->files_queue);
}

int main(int argc, char *argv[]) {
    struct cli_args args;
    int res = parse_cli_args(argc, argv, &args);

    if (res)
        return res;

    cleanup(&args);
    return 0;
}
