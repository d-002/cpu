#include "export_utils.h"

#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"

size_t get_extension_dot_index(char *path, bool *found)
{
    size_t index = 0;
    size_t i = 0;
    *found = false;
    while (path[i])
    {
        if (path[i] == '.')
        {
            index = i;
            *found = true;
        }
        i++;
    }

    return *found ? index : i;
}

char *change_extension(char *path, char *ext)
{
    bool found;
    size_t len = get_extension_dot_index(path, &found);
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
