#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "err.h"
#include "lexer/lexer.h"
#include "mystring.h"

TestSuite(Lexer);

Test(Lexer, Empty, .init = cr_redirect_stderr)
{
    struct string line = { "", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));
}

Test(Lexer, Space)
{
    struct string line = { " \n\t\r ", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, SPACE));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token->data), 0));

    token_destroy(token);
}

Test(Lexer, Opcode)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 1, &token), SUCCESS));
    cr_expect(eq(int, token->type, OPCODE));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "abcde"), 0));

    token_destroy(token);
}

Test(Lexer, Identifier)
{
    struct string line = { "abcde0_ fghij", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, IDENTIFIER));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "abcde0_"), 0));

    token_destroy(token);
}

Test(Lexer, Number)
{
    struct string line = { "0b1012", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_BIN));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "0b101"), 0));
    token_destroy(token);

    line.stream = "0x12ag";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_HEX));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "0x12a"), 0));
    token_destroy(token);

    line.stream = "123aa";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, NUMBER_DEC));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "123"), 0));
    token_destroy(token);
}

Test(Lexer, Data)
{
    struct string line = { "%r123", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, REGISTER));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token->data), 0));
    token_destroy(token);

    line.stream = "%m1234a";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, MEMORY));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "%m1234"), 0));
    token_destroy(token);

    line.stream = "%p00_ ";
    line.len = strlen(line.stream);
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, PORT));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "%p00"), 0));
    token_destroy(token);
}

Test(Lexer, Comment)
{
    struct string line = { "; sldkfjslk", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, COMMENT));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(line.stream, token->data), 0));

    token_destroy(token);
}

Test(Lexer, Simple)
{
    struct string line = { ".,:=;", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, DOT));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "."), 0));
    token_destroy(token);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, COMMA));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, ","), 0));
    token_destroy(token);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, COLON));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, ":"), 0));
    token_destroy(token);

    line.stream++;
    line.len--;
    cr_expect(eq(int, next_token(line, -1, 0, &token), SUCCESS));
    cr_expect(eq(int, token->type, EQUAL_SIGN));
    cr_expect(ne(ptr, token->data, NULL));
    cr_expect(eq(int, strcmp(token->data, "="), 0));
    token_destroy(token);
}

Test(Lexer, Unknown, .init = cr_redirect_stderr)
{
    struct string line = { "?", 0 };
    line.len = strlen(line.stream);
    struct token *token;

    cr_expect(eq(int, next_token(line, -1, 0, &token), LEXING_ERROR));
}

Test(Lexer, Errors, .init = cr_redirect_stderr)
{
    struct string line = { "0b", 0 };
    line.len = strlen(line.stream);
    struct token *token;

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
