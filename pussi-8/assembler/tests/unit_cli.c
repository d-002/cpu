#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "cli/cli.h"
#include "utils/errors.h"

TestSuite(Cli);

Test(Cli, Empty)
{
    struct cli_args args;
    cr_expect(eq(int, parse_cli_args(0, NULL, &args), SUCCESS));
    cr_expect(eq(int, args.run, 1));
    cr_expect(eq(int, args.verbose, 0));
    cli_args_destroy(&args);
}

Test(Cli, AllCorrect)
{
    struct cli_args args;
    char *argv[] = { NULL,      "--file", "hello.txt", "--verbose",
                     "--print", "--file", "world.md" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 SUCCESS));
    cr_expect(eq(int, args.run, 0));
    cr_expect(eq(int, args.verbose, 1));
    cr_expect(eq(int, args.files_queue->length, 2));
    cli_args_destroy(&args);
}

Test(Cli, Unknown, .init = cr_redirect_stderr)
{
    struct cli_args args;
    char *argv[] = { NULL, "--hello" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 CLI_ARGS_ERROR));
    cli_args_destroy(&args);
}

Test(Cli, FileNoArgument, .init = cr_redirect_stderr)
{
    struct cli_args args;
    char *argv[] = { NULL, "--file" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 CLI_ARGS_ERROR));
    cli_args_destroy(&args);
}
