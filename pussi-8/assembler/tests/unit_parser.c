#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "parser/instruction.h"
#include "parser/parser.h"
#include "unit_parser_utils.h"
#include "utils/errors.h"

TestSuite(Parser);

Test(Parser, Empty)
{
    init_current_string("");
    struct state *state = state_create(NULL);
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_expect(eq(int, state->instructions->length, 0));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, OnlyComment)
{
    init_current_string(";");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, IncorrectStart, .init = cr_redirect_stderr)
{
    init_current_string("=");
    struct state *state = state_create(NULL);
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
    struct state *state = state_create(NULL);
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_assert(eq(int, state->instructions->length, 1));

    cr_expect(ne(ptr, hash_map_get(state->labels, "a"), NULL));

    struct instruction *instruction = queue_dequeue(state->instructions);
    cr_expect(eq(str, instruction->opcode->data, "ADD"));
    cr_expect(eq(int, instruction->args_queue->length, 2));
    struct token *token = queue_dequeue(instruction->args_queue);
    cr_expect(eq(str, token->data, "%r1"));
    cr_expect(eq(int, token->type, REGISTER));
    token_destroy(token, 1);

    instruction_destroy(instruction);

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, LabelNoInstruction, .init = cr_redirect_stderr)
{
    init_current_string(".a");
    struct state *state = state_create(NULL);
    char *buf = NULL;

    cr_assert(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionWithIdentifier)
{
    init_current_string(".a sub %p0,variable;this is also a comment");
    struct state *state = state_create(NULL);
    char *buf = NULL;

    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    cr_assert(eq(int, state->instructions->length, 1));

    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionNoArgs)
{
    init_current_string("\tnop");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionEmptyLabel)
{
    init_current_string("\rnop");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionOneArg)
{
    init_current_string("  dummy 1");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, LabelNotAnIdentifier, .init = cr_redirect_stderr)
{
    init_current_string(".=:");
    struct state *state = state_create(NULL);
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
    struct state *state = state_create(NULL);
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
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    state_destroy(state);
    free_current_string();

    init_current_string(".a nop=1");
    state = state_create(NULL);
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, InstructionDupe, .init = cr_redirect_stderr)
{
    init_current_string(".a nop\n.a nop\n");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), VARS_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, TrailingData, .init = cr_redirect_stderr)
{
    init_current_string(" nop 0 0");
    struct state *state = state_create(NULL);
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
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_assert(eq(int, parse_lines(get_fake_line, NULL, state, &buf), SUCCESS));
    struct token *value = hash_map_get(state->vars, "a");
    cr_assert(ne(ptr, value, NULL));
    cr_expect(eq(str, value->data, "1"));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, AssignationDupe, .init = cr_redirect_stderr)
{
    init_current_string("a = 1\na = 2\n");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), VARS_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Parser, AssignationNoEqual, .init = cr_redirect_stderr)
{
    init_current_string("a : 1");
    struct state *state = state_create(NULL);
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
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), PARSING_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, DupeVarLabel, .init = cr_redirect_stderr)
{
    init_current_string("a=0\n.a:");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), VARS_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}

Test(Vars, DupeLabelVar, .init = cr_redirect_stderr)
{
    init_current_string(".a:\na=0");
    struct state *state = state_create(NULL);
    char *buf = NULL;
    cr_expect(
        eq(int, parse_lines(get_fake_line, NULL, state, &buf), VARS_ERROR));
    free(buf);
    state_destroy(state);
    free_current_string();
}
