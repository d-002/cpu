#include <criterion/criterion.h>
#include <criterion/new/assert.h>

TestSuite(Dummy);

Test(Dummy, HelloWorldTest) {
    cr_expect(true);
}
