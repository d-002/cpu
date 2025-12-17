#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "cli.h"
#include "err.h"
#include "queue.h"

TestSuite(Queue);

Test(Queue, QueueInit)
{
    struct queue *queue = queue_create();
    cr_expect(ne(ptr, queue, NULL));
    queue_destroy(queue);
}

Test(Queue, QueueEnqueue)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_expect(ne(ptr, queue, NULL));
    cr_expect(eq(int, queue_isempty(queue), 1));
    cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    cr_expect(eq(int, queue_isempty(queue), 0));
    queue_destroy(queue);
}

Test(Queue, QueueDequeue)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_expect(ne(ptr, queue, NULL));
    cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    cr_expect(eq(ptr, queue_dequeue(queue), data));
    free(data);
    cr_expect(eq(int, queue_isempty(queue), 1));
    queue_destroy(queue);
}

Test(Queue, QueueDequeueEmpty)
{
    struct queue *queue = queue_create();
    cr_expect(ne(ptr, queue, NULL));
    cr_expect(eq(ptr, queue_dequeue(queue), NULL));
    cr_expect(eq(int, queue_isempty(queue), 1));
    queue_destroy(queue);
}

Test(Queue, QueueComplex)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_expect(ne(ptr, queue, NULL));
    for (int i = 0; i < 10; i++)
        cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    for (int i = 0; i < 10; i++)
        cr_expect(eq(ptr, queue_dequeue(queue), data));
    free(data);

    cr_expect(eq(int, queue_isempty(queue), 1));
    queue_destroy(queue);
}

TestSuite(CliArgs);

Test(CliArgs, Empty)
{
    struct cli_args args;
    cr_expect(eq(int, parse_cli_args(0, NULL, &args), SUCCESS));
    cr_expect(eq(int, args.run, 1));
    cr_expect(eq(int, args.verbose, 0));
    cli_args_destroy(&args);
}

Test(CliArgs, AllCorrect)
{
    struct cli_args args;
    char *argv[] = { NULL,      "--file", "hello.txt", "--verbose",
                     "--print", "--file", "world.md" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 SUCCESS));
    cr_expect(eq(int, args.run, 0));
    cr_expect(eq(int, args.verbose, 1));
    cr_expect(eq(int, queue_isempty(args.files_queue), 0));
    cli_args_destroy(&args);
}

Test(CliArgs, Unknown, .init = cr_redirect_stderr)
{
    struct cli_args args;
    char *argv[] = { NULL, "--hello" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 CLI_ARGS_ERROR));
    cli_args_destroy(&args);
}

Test(CliArgs, FileNoArgument, .init = cr_redirect_stderr)
{
    struct cli_args args;
    char *argv[] = { NULL, "--file" };
    cr_expect(eq(int,
                 parse_cli_args(sizeof(argv) / sizeof(argv[0]), argv, &args),
                 CLI_ARGS_ERROR));
    cli_args_destroy(&args);
}
