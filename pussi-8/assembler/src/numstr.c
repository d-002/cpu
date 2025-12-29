#include "numstr.h"

#include <math.h>
#include <stdlib.h>

#include "logger.h"

char *itoa(int n)
{
    int len = n ? floor(log10(n)) + 1 : 1;

    char *res = malloc((len + 1) * sizeof(char));
    if (res == NULL)
        return NULL;

    res[len] = '\0';

    if (n)
    {
        int i = len - 1;
        while (n)
        {
            res[i--] = n % 10 + '0';
            n /= 10;
        }
    }
    else
        res[0] = '0';

    return res;
}

int atoi_base(int line, struct token *token)
{
    int n = 0;

    switch (token->type)
    {
    case NUMBER_DEC:
        return atoi(token->data);
    case NUMBER_BIN:
        for (size_t i = 2; i < token->length; i++)
            n = n * 2 + token->data[i] - '0';
        return n;
    case NUMBER_HEX:
        for (size_t i = 2; i < token->length; i++)
        {
            char c = token->data[i];
            n *= 16;
            if ('0' <= c && c <= '9')
                n += c - '0';
            else
                n += c - 'A' + 10;
        }
        return n;
    default:
        logerror(line, "Expected number but got %s", type2name(token->type));
        return -1;
    }
}
