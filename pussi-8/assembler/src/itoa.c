#include "itoa.h"

#include <math.h>
#include <stdlib.h>

char *itoa(int line)
{
    int len = line ? floor(log10(line)) + 1 : 1;

    char *res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
        return NULL;

    res[len] = '\0';

    if (line)
    {
        int i = len - 1;
        while (line)
        {
            res[i--] = line % 10 + '0';
            line /= 10;
        }
    }
    else
        res[0] = '0';

    return res;
}
