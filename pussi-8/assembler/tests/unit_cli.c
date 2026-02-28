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
    cr_expect(eq(int, args.print, 0));
    cr_expect(eq(int, args.verbose, 0));
    cli_args_destroy(&args);
}

Test(Cli, AllCorrect)
{
    struct cli_args args;
    char *argv[] = { NULL, "hello.txt", "--verbose", "--print", "world.md" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 SUCCESS));
    cr_expect(eq(int, args.run, 1));
    cr_expect(eq(int, args.print, 1));
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

Test(Cli, OnlyFiles)
{
    struct cli_args args;
    char *argv[] = { NULL, "file1", "file2", "file3" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 SUCCESS));
    cr_expect(eq(int, args.files_queue->length, 3));
    cli_args_destroy(&args);
}
