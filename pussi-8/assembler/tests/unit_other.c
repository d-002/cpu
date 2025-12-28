#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "itoa.h"

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
