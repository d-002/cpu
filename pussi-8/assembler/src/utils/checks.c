#include "checks.h"

#include "logger/logger.h"
#include "utils/errors.h"

int checks(void)
{
    if (sizeof(int) == 4 && sizeof(short) == 2)
        return SUCCESS;

    logerror(NO_LINE, "This program will not run on your system.");
    return OTHER_ERROR;
}
