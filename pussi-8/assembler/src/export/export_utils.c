#include "export_utils.h"

#include <stdlib.h>
#include <string.h>

#include "logger.h"

char *change_extension(char *path, char *ext)
{
    size_t index = 0;
    size_t i = 0;
    int found = 0;
    while (path[i])
    {
        if (path[i] == '.')
        {
            index = i;
            found = 1;
        }
        i++;
    }

    size_t len = found ? index : i;
    size_t ext_len = strlen(ext);

    char *res = malloc((len + ext_len + 2) * sizeof(char));
    if (res == NULL)
    {
        log_alloc_error(NO_LINE);
        return NULL;
    }

    memcpy(res, path, len);
    res[len] = '.';
    memcpy(res + len + 1, ext, ext_len);
    res[len + ext_len + 1] = '\0';

    return res;
}
