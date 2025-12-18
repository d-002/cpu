#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "cli.h"
#include "err.h"
#include "lexer/lexer.h"
#include "mystring.h"
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

TestSuite(Lexer);

Test(Lexer, Empty, .init = cr_redirect_stderr)
{
    struct string line = { "", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));
}

Test(Lexer, Space)
{
    struct string line = { " \n\t\r ", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, SPACE));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token.data), 0));

    free(token.data);
}

Test(Lexer, Opcode)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 1, &token), SUCCESS));
    cr_expect(eq(int, token.type, OPCODE));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "abcde"), 0));

    free(token.data);
}

Test(Lexer, Identifier)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, IDENTIFIER));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "abcde0_"), 0));

    free(token.data);
}

Test(Lexer, Number)
{
    struct string line = { "0b1012", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, NUMBER_BIN));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "0b101"), 0));
    free(token.data);

    line.stream = "0x12ag";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, NUMBER_HEX));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "0x12a"), 0));
    free(token.data);

    line.stream = "123aa";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, NUMBER_DEC));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "123"), 0));
    free(token.data);
}

Test(Lexer, Data)
{
    struct string line = { "%r123", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, REGISTER));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token.data), 0));
    free(token.data);

    line.stream = "%m1234a";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, MEMORY));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "%m1234"), 0));
    free(token.data);

    line.stream = "%p00_ ";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, PORT));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "%p00"), 0));
    free(token.data);
}

Test(Lexer, Comment)
{
    struct string line = { "; sldkfjslk", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, COMMENT));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token.data), 0));

    free(token.data);
}

Test(Lexer, Simple)
{
    struct string line = { ".,:=;", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, DOT));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "."), 0));
    free(token.data);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, COMMA));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, ","), 0));
    free(token.data);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, COLON));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, ":"), 0));
    free(token.data);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token.type, EQUAL_SIGN));
    cr_expect(ne(ptr, token.data, NULL));
    cr_expect(eq(int, strcmp(token.data, "="), 0));
    free(token.data);
}

Test(Lexer, Unknown, .init = cr_redirect_stderr)
{
    struct string line = { "?", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));
}

Test(Lexer, Errors, .init = cr_redirect_stderr)
{
    struct string line = { "0b", 0 };
    line.len = strlen(line.stream);
    struct token token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));

    line.stream = "%";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));

    line.stream = "%r";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));

    line.stream = "%s0";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));
}
