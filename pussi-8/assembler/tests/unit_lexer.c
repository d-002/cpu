#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "lexer/lexer.h"
#include "utils/errors.h"
#include "utils/mystring.h"

TestSuite(Lexer);

Test(Lexer, Empty, .init = cr_redirect_stderr)
{
    struct string line = { "", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, EOL));
    cr_expect(eq(ptr, token->data, NULL));
    cr_expect(eq(int, token->length, 0));
}

Test(Lexer, Space)
{
    struct string line = { " \n\t\r ", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, SPACE));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, line.stream, token->data));

    token_destroy(token, 1);
}

Test(Lexer, Opcode)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, true, &token), SUCCESS));
    cr_expect(eq(int, token->type, OPCODE));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "abcde"));

    token_destroy(token, 1);
}

Test(Lexer, Identifier)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, IDENTIFIER));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "abcde0_"));

    token_destroy(token, 1);
}

Test(Lexer, Number)
{
    struct string line = { "0b1012", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_BIN));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "0b101"));
    token_destroy(token, 1);

    line.stream = "0x12abg";
    line.length = strlen(line.stream);
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_HEX));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "0x12AB"));
    token_destroy(token, 1);

    line.stream = "123aa";
    line.length = strlen(line.stream);
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_DEC));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "123"));
    token_destroy(token, 1);
}

Test(Lexer, Data)
{
    struct string line = { "%r123", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, REGISTER));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, line.stream, token->data));
    token_destroy(token, 1);

    line.stream = "%m1234a";
    line.length = strlen(line.stream);
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, MEMORY));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "%m1234"));
    token_destroy(token, 1);

    line.stream = "%p00_ ";
    line.length = strlen(line.stream);
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, PORT));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "%p00"));
    token_destroy(token, 1);
}

Test(Lexer, Comment)
{
    struct string line = { "; sldkfjslk", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, COMMENT));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, line.stream, token->data));

    token_destroy(token, 1);
}

Test(Lexer, Simple)
{
    struct string line = { ".,:=;", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, DOT));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "."));
    token_destroy(token, 1);

    line.stream++;
    line.length--;
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, COMMA));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, ","));
    token_destroy(token, 1);

    line.stream++;
    line.length--;
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, COLON));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, ":"));
    token_destroy(token, 1);

    line.stream++;
    line.length--;
    cr_assert(eq(int, next_token(&line, -1, false, &token), SUCCESS));
    cr_expect(eq(int, token->type, EQUAL_SIGN));
    cr_assert(ne(ptr, token->data, NULL));
    cr_expect(eq(str, token->data, "="));
    token_destroy(token, 1);
}

Test(Lexer, Unknown, .init = cr_redirect_stderr)
{
    struct string line = { "?", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(&line, -1, false, &token), LEXING_ERROR));
}

Test(Lexer, Errors, .init = cr_redirect_stderr)
{
    struct string line = { "0b", 0 };
    line.length = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(&line, -1, false, &token), LEXING_ERROR));

    line.stream = "%";
    line.length = strlen(line.stream);
    cr_expect(eq(int, next_token(&line, -1, false, &token), LEXING_ERROR));

    line.stream = "%r";
    line.length = strlen(line.stream);
    cr_expect(eq(int, next_token(&line, -1, false, &token), LEXING_ERROR));

    line.stream = "%s0";
    line.length = strlen(line.stream);
    cr_expect(eq(int, next_token(&line, -1, false, &token), LEXING_ERROR));
}
