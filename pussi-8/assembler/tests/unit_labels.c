#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "assembler/expand_labels.h"
#include "parser/parser.h"
#include "unit_parser_utils.h"
#include "utils/errors.h"

TestSuite(Labels);

Test(Labels, Empty)
{
    init_current_string("");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_expect(eq(int, expand_labels(state), SUCCESS));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Labels, Single)
{
    init_current_string(".a\n jmp a");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_assert(eq(int, expand_labels(state), SUCCESS));

    char *value = hash_map_get(state->labels, "a");
    cr_assert(ne(ptr, value, NULL));
    cr_expect(eq(int, strcmp(value, "0"), 0));

    struct instruction *instruction = queue_iter_start(state->instructions);
    cr_assert(ne(ptr, instruction, NULL));
    struct token *arg = queue_iter_start(instruction->args_queue);
    cr_assert(ne(ptr, arg, NULL));
    cr_expect(eq(int, strcmp(arg->data, "0"), 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Labels, DoubleWithDifferingFakeLine)
{
    init_current_string(" add %r0,%r1\n.a\n jmp a\n.b jmp b");
    struct state *state = state_create();
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));

    cr_assert(eq(int, expand_labels(state), SUCCESS));

    char *value = hash_map_get(state->labels, "a");
    cr_assert(ne(ptr, value, NULL));
    cr_expect(eq(int, strcmp(value, "1"), 0));
    value = hash_map_get(state->labels, "b");
    cr_assert(ne(ptr, value, NULL));
    cr_expect(eq(int, strcmp(value, "3"), 0));

    struct instruction *instruction = queue_iter_start(state->instructions);
    instruction = queue_iter_next(state->instructions);
    cr_assert(ne(ptr, instruction, NULL));
    struct token *arg = queue_iter_start(instruction->args_queue);
    cr_assert(ne(ptr, arg, NULL));
    cr_expect(eq(int, strcmp(arg->data, "1"), 0));
    instruction = queue_iter_next(state->instructions);
    cr_assert(ne(ptr, instruction, NULL));
    arg = queue_iter_start(instruction->args_queue);
    cr_assert(ne(ptr, arg, NULL));
    cr_expect(eq(int, strcmp(arg->data, "3"), 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}
