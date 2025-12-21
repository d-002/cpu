#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "err.h"
#include "parser/parser.h"

TestSuite(Parser);

struct current_string
{
    char *str;
    size_t len;
    size_t pointer;
};

struct current_string current_string = {
    .str = NULL,
    .len = 0,
    .pointer = 0,
};

void init_current_string(char *str)
{
    size_t len = strlen(str);
    current_string.str = malloc(len + 1);
    current_string.len = len;
    current_string.pointer = 0;

    memcpy(current_string.str, str, len + 1);
}

void free_current_string(void)
{
    free(current_string.str);
    current_string.str = NULL;
    current_string.len = 0;
    current_string.pointer = 0;
}

ssize_t get_fake_line(char **buf_ptr, size_t *n,
                      __attribute__((unused)) FILE *stream)
{
    size_t i = current_string.pointer;
    while (i < current_string.len && current_string.str[i] != '\n')
        i++;

    if (i == current_string.len)
        i--;

    size_t len = i - current_string.pointer + 1;
    if (*buf_ptr == NULL || *n < len)
        *buf_ptr = realloc(*buf_ptr, len + 1);

    (*buf_ptr)[len] = '\0';
    memcpy(*buf_ptr, current_string.str + current_string.pointer, len);
    current_string.pointer += len;

    return len;
}

Test(Parser, Empty)
{
    init_current_string("");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, OnlyComment)
{
    init_current_string(";");
    struct state *state = state_create();
    char *buf = NULL;
    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, IncorrectStart, .init = cr_redirect_stderr)
{
    init_current_string("=");
    struct state *state = state_create();
    char *buf = NULL;
    cr_assert(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionWithLabel)
{
    init_current_string(".a: add %r1,%m2 ; this is a comment");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_assert(eq(int, state->instructions->length, 1));
    cr_assert(eq(int, state->labels->length, 1));

    struct label *label = queue_dequeue(state->labels);
    cr_expect(eq(int, strcmp(label->token->data, "a"), 0));

    struct instruction *instruction = queue_dequeue(state->instructions);
    cr_expect(eq(int, strcmp(instruction->opcode->data, "add"), 0));
    cr_expect(eq(int, instruction->args_list->length, 2));
    struct token *token = queue_dequeue(instruction->args_list);
    cr_expect(eq(int, strcmp(token->data, "%r1"), 0));
    cr_expect(eq(int, token->type, REGISTER));
    token_destroy(token, 1);

    label_destroy(label);
    instruction_destroy(instruction);

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionWithIdentifier)
{
    init_current_string(".a sub %p0,variable;this is also a comment");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_assert(eq(int, state->instructions->length, 1));
    cr_assert(eq(int, state->labels->length, 1));

    struct label *label = queue_dequeue(state->labels);
    cr_expect(eq(int, strcmp(label->token->data, "a"), 0));

    label_destroy(label);

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionNoArgs)
{
    init_current_string("\tnop");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionEmptyLabel)
{
    init_current_string("\rnop");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionOneArg)
{
    init_current_string("  dummy 1");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, LabelNotAnIdentifier, .init = cr_redirect_stderr)
{
    init_current_string(".=:");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionErrorAfterArg, .init = cr_redirect_stderr)
{
    init_current_string("  dummy 1,=");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionMissingSpace, .init = cr_redirect_stderr)
{
    init_current_string(".a=nop");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    state_destroy(state);
    free_current_string();

    init_current_string(".a nop=1");
    state = state_create();
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, TrailingData, .init = cr_redirect_stderr)
{
    init_current_string(" nop 0 0");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, Assignation)
{
    init_current_string("a = 1 ; ");
    struct state *state = state_create();
    char *buf = NULL;
    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    char *value = hash_map_get(state->vars, "a");
    cr_assert(ne(ptr, value, NULL));
    cr_expect(eq(int, strcmp(value, "1"), 0));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, AssignationDupe, .init = cr_redirect_stderr)
{
    init_current_string("a = 1\na = 2\n");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, AssignationNoEqual, .init = cr_redirect_stderr)
{
    init_current_string("a : 1");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, AssignationIncorrectArgument, .init = cr_redirect_stderr)
{
    init_current_string("a = :");
    struct state *state = state_create();
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}
