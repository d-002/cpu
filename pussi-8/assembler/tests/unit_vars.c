#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "assembler/expand_vars.h"
#include "err.h"
#include "parser/parser.h"
#include "unit_parser_utils.h"

TestSuite(Vars);

Test(Vars, Empty)
{
    init_current_string("");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_vars(state), SUCCESS));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, Simple)
{
    init_current_string("a = 1");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_vars(state), SUCCESS));

    struct token *token = hash_map_get(state->vars, "a");
    cr_assert(ne(ptr, token, NULL));
    cr_expect(eq(int, token->type, NUMBER_DEC));
    cr_expect(eq(int, strcmp(token->data, "1"), 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, DoubleRecursive)
{
    init_current_string("a = b\nc = b\nb = d\nd = 1\n");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_vars(state), SUCCESS));

    struct token *token = hash_map_get(state->vars, "a");
    cr_assert(ne(ptr, token, NULL));
    cr_expect(eq(int, token->type, NUMBER_DEC));
    cr_expect(eq(int, strcmp(token->data, "1"), 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, InfiniteRecursion, .init = cr_redirect_stderr)
{
    init_current_string("a = b\nb = a");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_vars(state), VARS_ERROR));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, UnknownMacro, .init = cr_redirect_stderr)
{
    init_current_string("a = b");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_vars(state), VARS_ERROR));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, ApplySimple)
{
    init_current_string(" add a,b\na=%r0\nb = %m1\nB = %p2");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_expect(eq(int, expand_vars(state), SUCCESS));

    struct instruction *instruction = state->instructions->head->data;
    cr_expect(eq(int, strcmp(instruction->opcode->data, "ADD"), 0));
    cr_expect(eq(int, instruction->args_queue->length, 2));
    struct token *arg1 = instruction->args_queue->head->data;
    struct token *arg2 = instruction->args_queue->tail->data;
    cr_expect(eq(int, strcmp(instruction->opcode->data, "ADD"), 0));
    cr_expect(eq(int, strcmp(arg1->data, "%r0"), 0));
    cr_expect(eq(int, strcmp(arg2->data, "%m1"), 0));
    cr_expect(eq(int, arg1->type, REGISTER));
    cr_expect(eq(int, arg2->type, MEMORY));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, UnknownButLabel)
{
    init_current_string(".a: jmp a");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_expect(eq(int, expand_vars(state), SUCCESS));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, UnknownVar, .init = cr_redirect_stderr)
{
    init_current_string(" add %r0,a");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_expect(eq(int, expand_vars(state), VARS_ERROR));

    free(buf);
    state_destroy(state);
    free_current_string();
}
