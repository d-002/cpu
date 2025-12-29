#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "numstr.h"

TestSuite(Other);

Test(Other, Itoa0)
{
    char *s = itoa(0);
    cr_assert(ne(ptr, s, NULL));
    cr_expect(eq(str, s, "0"));

    free(s);
}

Test(Other, ItoaSimple)
{
    char *s = itoa(42);
    cr_assert(ne(ptr, s, NULL));
    cr_expect(eq(str, s, "42"));

    free(s);
}

Test(Other, AtoiBaseDec)
{
    struct token *token = token_create(NUMBER_DEC, "42", 2);
    cr_assert(ne(ptr, token, NULL));
    int n = atoi_base(0, token);
    cr_expect(eq(int, n, 42));

    token_destroy(token, 1);
}

Test(Other, AtoiBaseBin)
{
    struct token *token = token_create(NUMBER_BIN, "0b101010", 8);
    cr_assert(ne(ptr, token, NULL));
    int n = atoi_base(0, token);
    cr_expect(eq(int, n, 42));

    token_destroy(token, 1);
}

Test(Other, AtoiBaseHex)
{
    struct token *token = token_create(NUMBER_HEX, "0xBEEF0", 7);
    cr_assert(ne(ptr, token, NULL));
    int n = atoi_base(0, token);
    cr_expect(eq(int, n, 782064));

    token_destroy(token, 1);
}

Test(Other, AtoiBaseWrong, .init = cr_redirect_stderr)
{
    struct token *token = token_create(EOL, "", 0);
    cr_assert(ne(ptr, token, NULL));
    int n = atoi_base(0, token);
    cr_expect(eq(int, n, -1));

    token_destroy(token, 1);
}
