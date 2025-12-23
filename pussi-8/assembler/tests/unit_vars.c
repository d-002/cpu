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
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

    cr_expect(eq(int, expand_vars(state), 0));

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
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

    cr_expect(eq(int, expand_vars(state), 0));

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
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

    cr_expect(eq(int, expand_vars(state), 0));

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
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

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
    cr_expect(eq(int, state->instructions->length, 0));
    cr_expect(eq(int, state->labels->length, 0));

    cr_expect(eq(int, expand_vars(state), VARS_ERROR));

    free(buf);
    state_destroy(state);
    free_current_string();
}
